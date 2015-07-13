/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-06-25
 *      Updated:
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  exceptions_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       File created
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"



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

        @ Handle interrupts
        movw    sp, #:lower16:__stack_irq_top
        movt    sp, #:upper16:__stack_irq_top
        
        @ - Get the number of the highest priority active IRQ
        movw    r0, #:lower16:INTC_BASE
        movt    r0, #:upper16:INTC_BASE
        ldr     r1, [r0, #INTC_SIR_IRQ_OFFSET]
        and     r1, r1, #INTC_ACTIVEIRQ_MASK
        
        cmp     r1, #INTC_INT_NUMBER_TIMER2_PAT
        bne     irq_assert
        mov     r0, #2                                      @ set the old process in state ready
        bl      _proc_ctrl_schedule                         @ schedule a "new" process to run
        
        @ - Update the timer
        movw    r0, #:lower16:DM_TIMER2_BASE
        movt    r0, #:upper16:DM_TIMER2_BASE
        mov     r1, #TIMER_IRQSTATUS_MAT_IT_FLAG_MASK
        str     r1, [r0, #TIMER_IRQSTATUS_OFFSET]           @ clear the interrupt
        mov     r1, #TCRR_INIT_VALUE_PAT
        str     r1, [r0, #TIMER_TCRR_OFFSET]                @ Set the timer counter to the start value

        @ - Allow new IRQs/FIQs at INTC side
        @ The INTC_CONTROL register is a write only register so no need to write back others bits
        movw    r0, #:lower16:INTC_BASE
        movt    r0, #:upper16:INTC_BASE
        mov     r1, #INTC_NEW_IRQ_AGR_MASK
        str     r1, [r0, #INTC_CONTROL_OFFSET]              @ Write the INTC_NEW_IRQ_AGR_MASK bit to allow new IRQs

        @ - Data Synchronization Barrier
        mov     r0, #0x0
        mcr     p15, #0, r0, c7, c10, #4
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
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:irq_assert_print_out$
        movt    r0, #:upper16:irq_assert_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
irq_assert_print_out$:
.asciz "IRQ ASSERT \r\n"
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
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:fiq_handl_print_out$
        movt    r0, #:upper16:fiq_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
fiq_handl_print_out$:
.asciz "EXCEPTION: FIQ Exception (not handled...).\r\nAt exception current PCB: \r\n"
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
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:undef_instr_handl_print_out$
        movt    r0, #:upper16:undef_instr_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
undef_instr_handl_print_out$:
.asciz "EXCEPTION: Undefined Instruction Exception (not handled...)\r\nAt exception current PCB: \r\n"
.align 2


         /*********************************************************************** 
         * svc_handl
         *
         *  Notification on a Supervisor Call exception.
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
        mrs     r0, spsr
        stmia   r13, {r0, r14}

        @ Notify on the exception
        movw    sp, #:lower16:__stack_svc_top
        movt    sp, #:upper16:__stack_svc_top
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:svc_handl_print_out$
        movt    r0, #:upper16:svc_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
svc_handl_print_out$:
.asciz "EXCEPTION: Supervisor Call Exception (not handled...)\r\nAt exception current PCB: \r\n"
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
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:prefetch_abort_handl_print_out$
        movt    r0, #:upper16:prefetch_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
prefetch_abort_handl_print_out$:
.asciz "EXCEPTION: Prefetch Abort Exception (not handled...)\r\nAt exception current PCB: \r\n"
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
        movw    r4, #:lower16:puts
        movt    r4, #:upper16:puts
        ldr     r4, [r4]
        movw    r0, #:lower16:data_abort_handl_print_out$
        movt    r0, #:upper16:data_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        
        @ Hang the system
        b       _deadloop
data_abort_handl_print_out$:
.asciz "EXCEPTION: Data Abort Exception (not handled...)\r\nAt exception current PCB: \r\n"
.align 2







