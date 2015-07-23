/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-06-20
 *      Updated:    2015-06-01
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
        ldr     r4, =puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =undef_instr_handl_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
undef_instr_handl_print_out$:
.asciz "EXCEPTION: Undefined Instruction Exception (not handled...)\r\n"
.align 2


         /*********************************************************************** 
         * _swi_handl
         *
         *  Notification on a Software Interrupt exception.
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
        
        @ Clear timer channel
        ldr     r1, =SYS_TIMER_CS_M1_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        
        @ to be added: remove the timer interrupt comming from the GPU...

10:
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
        ldr     r4, =puts                           @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =swi_not_handl_print_out$
        mov     r1, #100
        blx     r4
        bl      proc_ctrl_print_curr_pcb
        b       _deadloop
swi_not_handl_print_out$:
.asciz "\r\n\r\nEXCEPTION: Software Interrupt Exception (not handled...)\r\n\r\n"
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
        ldr     r4, =puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =prefetch_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
prefetch_abort_handl_print_out$:
.asciz "EXCEPTION: Prefetch Abort Exception (not handled...)\r\n"
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
        ldr     r4, =puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =data_abort_handl_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
data_abort_handl_print_out$:
.asciz "EXCEPTION: Data Abort Exception (not handled...)\r\n"
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

        @ is the timer intrrupt from GPU?
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        and     r1, r1, #SYS_TIMER_CS_M1_MASK
        cmp     r1, #SYS_TIMER_CS_M1_MASK
        bne     10f

        @ Handle interrupts
        ldr     sp, =__stack_exc_irq_top
        @ldr     r4, =puts                   @ as of now, just print out a note...
        @ldr     r4, [r4]
        @ldr     r0, =irq_print_out$
        @mov     r1, #100
        @blx     r4
        mov     r0, #2                      @ set the old process in state ready
        bl      _proc_ctrl_schedule         @ schedule a new process to run
        
        @ Add a new timeout
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_INTR_INTERVAL_PAT
        str     r1, [r0, #SYSTEM_TIMER_C1_OFFSET]
        
        @ Clear timer channel
        ldr     r1, =SYS_TIMER_CS_M1_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        
        @ to be added: remove the timer interrupt comming from the GPU...

10:
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
.asciz "System timer has expired - system has been interrupted... \r\n"
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
        ldr     r4, =puts                   @ as of now, just print out a note...
        ldr     r4, [r4]
        ldr     r0, =fiq_handl_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
fiq_handl_print_out$:
.asciz "EXCEPTION: FIQ Exception (not handled...)\r\n"
.align 2


