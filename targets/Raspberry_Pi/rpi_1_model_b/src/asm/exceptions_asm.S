/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-06-20
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  exceptions_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       Updated
 *      ver 8       Updated
 *      ver 9       Added methods for a processes to hand over to an other 
 *                  process
 *      ver 10      Enabled UART Rx interrupts and adjusted for handling 
 *                  I/O buffers.
 *                  Updated handling of unknown exceptions.
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
 #include "hardware_system.h"
 #include "sys_def.h"
 
 
 
         /*********************************************************************** 
         * _undef_instr_handl
         *
         *  Notification on a Undefined Instruction exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _undef_instr_handl
_undef_instr_handl:
        @ Disable IRQ/FIQ on MPU
        mrs     r13, cpsr
        orr     r13, r13, #(CPSR_IRQ_BM | CPSR_FIQ_BM)
        msr     cpsr_c, r13
        
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r0, spsr
        stmia   r13, {r0, r14}
        
        @ restore stack pointer
        ldr     sp, =__stack_exc_und_top

        @ Notify the user about the exception
        ldr     r4, =_puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =undef_instr_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop
undef_instr_handl_print_out$:
.asciz "EXCEPTION: Undefined Instruction Exception (not handled...)\n"
.align 2


         /*********************************************************************** 
         * _swi_handl
         *
         *  Handle software interrupt requests.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _swi_handl
_swi_handl:
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r1, spsr
        add     r14, r14, #4                        @ Adjust lr, compared with an irq, due to they are
                                                    @ handled in different stage in the pipelining
        stmia   r13, {r1, r14}
        
        @ Handle SWI call
        ldr     sp, =__stack_exc_irq_top
        ldr     r1, [lr, #-8]
        bic     r1, r1, #0xff000000                 @ Get the SWI number
        cmp     r1, #SYSTEM_CALL_CONTEXT_SWITCH
        bne     swi_not_handled$
        bl      _proc_ctrl_schedule                 @ schedule a new process to run (r0 contain the return state)

        @ Add a new timeout
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_INTR_INTERVAL_PAT
        str     r1, [r0, #SYSTEM_TIMER_C1_OFFSET]
        
        @ Clear timer channel (M1)
        ldr     r1, =SYS_TIMER_CS_M1_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        
        @ Disturbe the timer interrupt comming from the GPU so they not affect the MPU interrupts...
        /*ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_GPU_DISTURB_PAT
        str     r1, [r0, #SYSTEM_TIMER_C0_OFFSET]
        str     r1, [r0, #SYSTEM_TIMER_C2_OFFSET]
        
        @ Clear timer channel (M0 + M2)
        ldr     r1, =SYS_TIMER_CS_M0_MASK | SYS_TIMER_CS_M2_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]*/
        
        @b       100f
        
        @bl      _proc_ctrl_print_status

100:
        @ Fetch registers from PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        add     r13, r13, #68
        ldmdb   r13!, {r0, r14}
        msr     spsr_cxsf, r0
        ldmdb   r13, {r0 - r14}^
        nop
        subs    pc, r14, #4

swi_not_handled$:
        ldr     r4, =_puts                           @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =swi_not_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop
swi_not_handl_print_out$:
.asciz "\n\nEXCEPTION: Software Interrupt Exception (not handled...)\n\n"
.align 2


         /*********************************************************************** 
         * _prefetch_abort_handl
         *
         *  Notification on a Prefetch Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _prefetch_abort_handl
_prefetch_abort_handl:
        @ Disable IRQ/FIQ on MPU
        mrs     r13, cpsr
        orr     r13, r13, #(CPSR_IRQ_BM | CPSR_FIQ_BM)
        msr     cpsr_c, r13
        
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r0, spsr
        stmia   r13, {r0, r14}
        
        @ restore stack pointer
        ldr     sp, =__stack_exc_abt_top

        @ Notify the user about the exception
        ldr     r4, =_puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =prefetch_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop
prefetch_abort_handl_print_out$:
.asciz "EXCEPTION: Prefetch Abort Exception (not handled...)\n"
.align 2


         /*********************************************************************** 
         * _data_abort_handl
         *
         *  Notification on a Data Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _data_abort_handl
_data_abort_handl:
        @ Disable IRQ/FIQ on MPU
        mrs     r13, cpsr
        orr     r13, r13, #(CPSR_IRQ_BM | CPSR_FIQ_BM)
        msr     cpsr_c, r13
        
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r0, spsr
        stmia   r13, {r0, r14}
        
        @ restore stack pointer
        ldr     sp, =__stack_exc_abt_top

        @ Notify the user about the exception
        ldr     r4, =_puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =data_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop
data_abort_handl_print_out$:
.asciz "EXCEPTION: Data Abort Exception (not handled...)\n"
.align 2


         /*********************************************************************** 
         * _irq_handl
         *
         * Handle interrupts requests.
         *
         * C prototype: N/A 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _irq_handl
_irq_handl:
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r0, spsr
        stmia   r13, {r0, r14}
        
        @ restore stack pointer
        ldr     sp, =__stack_exc_irq_top

        @ if (timer) interrupt due to a need of context switch
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        and     r1, r1, #SYS_TIMER_CS_M1_MASK
        cmp     r1, #SYS_TIMER_CS_M1_MASK
        beq     10f
        
        @ if interrupt due to a terminal key press
        ldr     r0, =INTR_BASE
        ldr     r1, [r0, #INTR_IRQ_PEND_2_OFFSET]
        and     r1, r1, #INTR_UART_RX_PEND_MASK
        cmp     r1, #INTR_UART_RX_PEND_MASK
        beq     20f
        
        @ Unhandled interrupt
        b       99f

10:
        @ Handle interrupts due to context switch
        mov     r0, #2                      @ set the old process in state ready
        bl      _proc_ctrl_schedule         @ schedule a new process to run

        @ Add a new timeout
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_INTR_INTERVAL_PAT
        str     r1, [r0, #SYSTEM_TIMER_C1_OFFSET]
        
        @ Clear timer channel (M1)
        ldr     r1, =SYS_TIMER_CS_M1_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        
        @bl      _proc_ctrl_print_status
        
        @ Disturb the timer interrupt coming from the GPU so they not affect the MPU interrupts...
        /*ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_GPU_DISTURB_PAT
        str     r1, [r0, #SYSTEM_TIMER_C0_OFFSET]
        str     r1, [r0, #SYSTEM_TIMER_C2_OFFSET]
        
        @ Clear timer channel (M0 + M2)
        ldr     r1, =SYS_TIMER_CS_M0_MASK | SYS_TIMER_CS_M2_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]*/
        
        b       100f
        
20:
        @ Handle interrupt due to a key press on the terminal
        ldr     r4, =_getc
        ldr     r4, [r4]
        ldr     r5, =_putc
        ldr     r5, [r5]
21:
        @ get the char and put it back again...
        blx     r4                  @ _getc
        and     r6, r0, #0xFF
        blx     r5                  @ _putc
        @mov r1, #1
        @bl      _print_hex32
22:
        @ '\n' is used as line ending in this OS (the below might need to 
        @ be changed due to your terminal settings...)
        @ if ('\r' == r6) r6 = '\n'
        cmp     r6, #'\r'
        bne     23f
        
        @ set the io_in_process in ready state, there is a new line to process...
        mov     r0, #PID_CLI_IO_IN
        mov     r1, #2                      @ state ready = 2...
        bl      proc_ctrl_change_state

        mov     r0, #'\n'
        blx     r5
        mov     r6, #'\n'
23:
        @ put the char in the "stdin" buffer
        ldr     r0, =io_in
        mov     r1, r6
        bl      _io_buffer_write_char
24:
        @ Is Rx FIFO empty?
        /*ldr     r0, = UART_BASE
        ldr     r1, [r0, #UART_FR_OFFSET]
        and     r1, r1, #UART_FR_RX_FIFO_EMPTY_MASK
        cmp     r1, #UART_FR_RX_FIFO_EMPTY_MASK
        beq     21b

        b       100f */

99:
        @ Notify the user about the unknown interrupt
        /*ldr     r4, =_puts                  @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =irq_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop*/

100:
        @ Fetch registers from PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        add     r13, r13, #68
        ldmdb   r13!, {r0, r14}
        msr     spsr_cxsf, r0
        ldmdb   r13, {r0 - r14}^
        nop
        subs    pc, r14, #4
irq_print_out$:
.asciz "System timer has expired - system has been interrupted... \n"
.align 2
irq_handl_print_out$:
.asciz "EXCEPTION: IRQ Exception (not handled...)\n"
.align 2


         /*********************************************************************** 
         * _fiq_handl
         *
         *  Notification on a Data Abort exception.
         *
         * C prototype: N/A
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _fiq_handl
_fiq_handl:
        @ Disable IRQ/FIQ on MPU
        mrs     r13, cpsr
        orr     r13, r13, #(CPSR_IRQ_BM | CPSR_FIQ_BM)
        msr     cpsr_c, r13
        
        @ Store registers in PCB
        ldr     r13, =gpCurrPCB
        ldr     r13, [r13]
        stmia   r13!, {r0 - r14}^
        mrs     r0, spsr
        stmia   r13, {r0, r14}
        
        @ restore stack pointer
        ldr     sp, =__stack_exc_fiq_top

        @ Notify the user about the exception
        ldr     r4, =_puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =fiq_handl_print_out$
        mov     r1, #100
        blx     r4
        blx      proc_ctrl_print_curr_pcb
        b       _deadloop
fiq_handl_print_out$:
.asciz "EXCEPTION: FIQ Exception (not handled...)\n"
.align 2


