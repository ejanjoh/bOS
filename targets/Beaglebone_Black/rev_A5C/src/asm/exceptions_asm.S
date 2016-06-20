/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-06-25
 *      Updated:    2016-06-16
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  exceptions_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       File created
 *      ver 9       Added methods for a processes to hand over to an other 
 *                  process
 *      ver 10      Added UART0 Rx interrupt
 *      ver 13      Updated due to a change in process_control.c
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"
#include "sys_def.h"
 

        /***********************************************************************
         * redirect_exception_vectors
         *
         *  Redirect the exception vectors. Change the default addresses loaded 
         *  at the jump addresses.
         *
         * C prototype: void redirect_exception_vectors(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global redirect_exception_vectors
redirect_exception_vectors:

        addr        .req r0
        excHandl    .req r1

        @ clear the A bit - enable asynchronous aborts
        /*mrs     r2, cpsr
        bic     r2, r2, #(1 << 8)
        msr     cpsr, r1*/

        @ Undefined Instruction
        movw    addr, #:lower16:LOAD_ADDR_UNDEF_INSTR_EXC_REG
        movt    addr, #:upper16:LOAD_ADDR_UNDEF_INSTR_EXC_REG
        movw    excHandl, #:lower16:undef_instr_handl
        movt    excHandl, #:upper16:undef_instr_handl
        str     excHandl, [addr]

        @ Supervisor Call
        movw    addr, #:lower16:LOAD_ADDR_SVC_EXC_REG
        movt    addr, #:upper16:LOAD_ADDR_SVC_EXC_REG
        movw    excHandl, #:lower16:svc_handl
        movt    excHandl, #:upper16:svc_handl
        str     excHandl, [addr]

        @ Prefetch Abort
        movw    addr, #:lower16:LOAD_ADDR_PREF_ABRT_EXC_REG
        movt    addr, #:upper16:LOAD_ADDR_PREF_ABRT_EXC_REG
        movw    excHandl, #:lower16:prefetch_abort_handl
        movt    excHandl, #:upper16:prefetch_abort_handl
        str     excHandl, [addr]

        @ Data Abort
        movw    addr, #:lower16:LOAD_ADDR_DATA_ABRT_EXC_REG
        movt    addr, #:upper16:LOAD_ADDR_DATA_ABRT_EXC_REG
        movw    excHandl, #:lower16:data_abort_handl
        movt    excHandl, #:upper16:data_abort_handl
        str     excHandl, [addr]

        @ IRQ Interrupt
        movw    addr, #:lower16:LOAD_ADDR_IRQ_INTR_REG
        movt    addr, #:upper16:LOAD_ADDR_IRQ_INTR_REG
        movw    excHandl, #:lower16:irq_handl
        movt    excHandl, #:upper16:irq_handl
        str     excHandl, [addr]

        @ FIQ Interrupt
        movw    addr, #:lower16:LOAD_ADDR_FIQ_INTR_REG
        movt    addr, #:upper16:LOAD_ADDR_FIQ_INTR_REG
        movw    excHandl, #:lower16:fiq_handl
        movt    excHandl, #:upper16:fiq_handl
        str     excHandl, [addr]

        mov     pc, lr
        .unreq  addr
        .unreq  excHandl


         /*********************************************************************** 
         * irq_handl
         *
         * Handle interrupts requests.
         *
         * C prototype: N/A 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global irq_handl
irq_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r0, spsr
        stmia   r13, {r0, r14}
10:
        @ Handle interrupts
        movw    sp, #:lower16:__stack_irq_top
        movt    sp, #:upper16:__stack_irq_top
20:
        @ - Get the number of the highest priority active IRQ
        movw    r0, #:lower16:INTC_BASE
        movt    r0, #:upper16:INTC_BASE
        ldr     r1, [r0, #INTC_SIR_IRQ_OFFSET]
        and     r1, r1, #INTC_ACTIVEIRQ_MASK
30:
        cmp     r1, #INTC_INT_NUMBER_TIMER2_PAT
        beq     40f
        cmp     r1, #INTC_INT_NUMBER_UART0_PAT
        beq     50f
        b       irq_assert
40:
        @ - Timer interrupt (context switch)
        mov     r0, #2                                      @ set the old process in state ready
        bl      _proc_ctrl_schedule                         @ schedule a "new" process to run

        @ - Update the timer
        movw    r0, #:lower16:DM_TIMER2_BASE
        movt    r0, #:upper16:DM_TIMER2_BASE
        mov     r1, #TIMER_IRQSTATUS_MAT_IT_FLAG_MASK
        str     r1, [r0, #TIMER_IRQSTATUS_OFFSET]           @ clear the interrupt
        mov     r1, #TCRR_INIT_VALUE_PAT
        str     r1, [r0, #TIMER_TCRR_OFFSET]                @ Set the timer counter to the start value

        b       60f
50:
        movw    r4, #:lower16:UART0_BASE
        movt    r4, #:upper16:UART0_BASE
 
        @ Check the root for the UART interrupt
        ldrh    r0, [r4, #UART_IIR_OFFSET]
        cmp     r0, #UART_IIR_IT_RHR_PEND_MASK
        beq     51f                                         @ if Rx interrupt
        mov     r5, r0
        
        @ Notify about the unknown cause to the interrupt
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:unknown_uart_it$
        movt    r0, #:upper16:unknown_uart_it$
        mov     r1, #100
        blx     r4
        mov     r0, r5
        mov     r1, #1
        bl      _print_hex32
        b       60f
51:
        @ Rx (RHR) input interrupt
        movw    r4, #:lower16:_getc
        movt    r4, #:upper16:_getc
        ldr     r4, [r4]
        movw    r5, #:lower16:_putc
        movt    r5, #:upper16:_putc
        ldr     r5, [r5]
52:
        @ get the char and put it back again...
        blx     r4                  @ _getc
        and     r6, r0, #0xFF
        blx     r5                  @ _putc
53:
        @ '\n' is used as line ending in this OS (the below might need to 
        @ be changed due to your terminal settings...)
        @ if ('\r' == r6) r6 = '\n';
        cmp     r6, #'\r'
        bne     54f
        
        @ set the io_in_process in ready state, there is a new line to process...
        mov     r0, #PID_CLI_IO_IN
        mov     r1, #2              @ state ready = 2...
        bl      _proc_ctrl_change_state

        mov     r0, #'\n'
        blx     r5                  @ _putc
        mov     r6, #'\n'
54:
        @ put the char in the "stdin" buffer
        ldr     r0, =io_in
        mov     r1, r6
        bl      _io_buffer_write_char
/*
55:
        @ - If still interrupted, clear the RHR interrupt by reading the RHR register continuously
        ldrh    r0, [r4, #UART_IIR_OFFSET]
        and     r0, r0, #UART_IIR_IT_PEND_MASK
        cmp     r0, #UART_IIR_IT_PEND_MASK
        bne     55b
*/
60:
        @ - Allow new IRQs/FIQs at INTC side
        @ The INTC_CONTROL register is a write only register so no need to write back others bits
        movw    r0, #:lower16:INTC_BASE
        movt    r0, #:upper16:INTC_BASE
        mov     r1, #INTC_NEW_IRQ_AGR_MASK
        str     r1, [r0, #INTC_CONTROL_OFFSET]              @ Write the INTC_NEW_IRQ_AGR_MASK bit to allow new IRQs

        @ - Data Synchronization Barrier
        mov     r0, #0x0
        mcr     p15, #0, r0, c7, c10, #4  
100:
        @ Fetch registers from PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        add     r13, r13, #68
        ldmdb   r13!, {r0, r14}
        msr     spsr_cxsf, r0
        ldmdb   r13, {r0 - r14}^
        nop
        subs    pc, lr, #4

unknown_uart_it$:
.asciz "UART interrupt, unknown source: "
.align 2


         /*********************************************************************** 
         * irq_assert
         *
         *  Handle an assert during a irq
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
irq_assert:
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:irq_assert_print_out$
        movt    r0, #:upper16:irq_assert_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
irq_assert_print_out$:
.asciz "IRQ ASSERT \n"
.align 2


         /*********************************************************************** 
         * fiq_handl
         *
         *  Notification on a Data Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global fiq_handl
fiq_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r0, spsr
        stmia   r13, {r0, r14}

        @ Notify on the exception
        movw    sp, #:lower16:__stack_fiq_top
        movt    sp, #:upper16:__stack_fiq_top
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:fiq_handl_print_out$
        movt    r0, #:upper16:fiq_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
fiq_handl_print_out$:
.asciz "EXCEPTION: FIQ Exception (not handled...).\nAt exception current PCB: \n"
.align 2


         /*********************************************************************** 
         * undef_instr_handl
         *
         *  Notification on a Undefined Instruction exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global undef_instr_handl
undef_instr_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r0, spsr
        stmia   r13, {r0, r14}

        @ Notify on the exception
        movw    sp, #:lower16:__stack_und_top
        movt    sp, #:upper16:__stack_und_top
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:undef_instr_handl_print_out$
        movt    r0, #:upper16:undef_instr_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
undef_instr_handl_print_out$:
.asciz "EXCEPTION: Undefined Instruction Exception (not handled...)\nAt exception current PCB: \n"
.align 2


         /*********************************************************************** 
         * svc_handl
         *
         *  Handle supervisor call requests.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global svc_handl
svc_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r1, spsr
        add     r14, r14, #4                            @ Adjust lr, compared with an irq, due to they are
                                                        @ handled in different stage in the pipelining
        stmia   r13, {r1, r14}

        @ Handle svc call
        movw    sp, #:lower16:__stack_svc_top
        movt    sp, #:upper16:__stack_svc_top
        ldr     r1, [lr, #-8]
        bic     r1, r1, #0xff000000                     @ Get the svc number
        cmp     r1, #SYSTEM_CALL_CONTEXT_SWITCH
        bne     svc_not_handled$
        bl      _proc_ctrl_schedule                     @ schedule a new process to run (r0 contain the return state)

        @ Update the timer
        movw    r0, #:lower16:DM_TIMER2_BASE
        movt    r0, #:upper16:DM_TIMER2_BASE
        mov     r1, #TIMER_IRQSTATUS_MAT_IT_FLAG_MASK
        str     r1, [r0, #TIMER_IRQSTATUS_OFFSET]       @ clear the interrupt
        mov     r1, #TCRR_INIT_VALUE_PAT
        str     r1, [r0, #TIMER_TCRR_OFFSET]            @ Set the timer counter to the start value

10:
        @ Fetch registers from PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        add     r13, r13, #68
        ldmdb   r13!, {r0, r14}
        msr     spsr_cxsf, r0
        ldmdb   r13, {r0 - r14}^
        nop
        subs    pc, lr, #4

svc_not_handled$:
        @ Notify on the exception
        movw    sp, #:lower16:__stack_svc_top
        movt    sp, #:upper16:__stack_svc_top
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:svc_handl_print_out$
        movt    r0, #:upper16:svc_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
svc_handl_print_out$:
.asciz "EXCEPTION: Supervisor Call Exception (not handled...)\nAt exception current PCB: \n"
.align 2


         /*********************************************************************** 
         * prefetch_abort_handl
         *
         *  Notification on a Prefetch Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global prefetch_abort_handl
prefetch_abort_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r0, spsr
        stmia   r13, {r0, r14}

        @ Notify on the exception
        movw    sp, #:lower16:__stack_abt_top
        movt    sp, #:upper16:__stack_abt_top
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:prefetch_abort_handl_print_out$
        movt    r0, #:upper16:prefetch_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
prefetch_abort_handl_print_out$:
.asciz "EXCEPTION: Prefetch Abort Exception (not handled...)\r\nAt exception current PCB: \n"
.align 2


         /*********************************************************************** 
         * data_abort_handl
         *
         *  Notification on a Data Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global data_abort_handl
data_abort_handl:
        @ Store current registers in PCB
        movw    r13, #:lower16:gpCurrPCB
        movt    r13, #:upper16:gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        /* Note: For some reason "stmia   r13!, {r0 - r14}^" does not increment the r13/sp register after
                 operation! Check up why... Until this has been solved we increment explicitly. */
        add     r13, r13, #(4*15)
        mrs     r0, spsr
        stmia   r13, {r0, r14}

        @ Notify on the exception
        movw    sp, #:lower16:__stack_abt_top
        movt    sp, #:upper16:__stack_abt_top
        movw    r4, #:lower16:_puts
        movt    r4, #:upper16:_puts
        ldr     r4, [r4]
        movw    r0, #:lower16:data_abort_handl_print_out$
        movt    r0, #:upper16:data_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
data_abort_handl_print_out$:
.asciz "EXCEPTION: Data Abort Exception (not handled...)\r\nAt exception current PCB: \n"
.align 2







