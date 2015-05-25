/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-19
 *      Updated:
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  start.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File created
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"



        /*********************************************************************** 
         * _start
         *
         * Global start of the software system. 
         *
         * @ address 0x402F0400: start of .init section and at boot this is the 
         * first code running in bOS, i.e. bOS system start.
         *
         * C prototype: N/A 
         **********************************************************************/
        .section .init
        .code 32
        .align 2
        .global _start
_start:
        add     r10, pc, #-0x8                  @ r10 = current load address
        
        /* User the memory area reserved for the heap as a temporary stack while 
         * the real stack is filled with watermarks (supervisor mode)*/
        movw    sp, #:lower16:__heap_end
        movt    sp, #:upper16:__heap_end

        @ Put a watermark onto the full stack area
        movw    r0, #:lower16:_stack_start
        movt    r0, #:upper16:_stack_start
        ldr     r1, =STACK_WATERMARK
        movw    r2, #:lower16:_stack_end
        movt    r2, #:upper16:_stack_end
        sub     r2, r2, r0
        lsr     r2, r2, #2
        add     r2, r2, #1
        bl      memset32 

        @ Assign a stack pointer to supervisor mode
        movw    sp, #:lower16:__stack_svc_top
        movt    sp, #:upper16:__stack_svc_top

        @ Change to undefined mode
        mov     r0, #UND32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to undefined mode
        movw    sp, #:lower16:__stack_und_top
        movt    sp, #:upper16:__stack_und_top

        @ Change to abort mode
        mov     r0, #ABT32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to abort mode
        movw    sp, #:lower16:__stack_abt_top
        movt    sp, #:upper16:__stack_abt_top

        @ Change to irq mode
        mov     r0, #IRQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to irq mode
        movw    sp, #:lower16:__stack_irq_top
        movt    sp, #:upper16:__stack_irq_top

        @ Change to fiq mode
        mov     r0, #FIQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to fiq mode
        movw    sp, #:lower16:__stack_fiq_top
        movt    sp, #:upper16:__stack_fiq_top

        @ Change to system mode
        mov     r0, #SYS32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to system mode
        movw    sp, #:lower16:__stack_sys_top
        movt    sp, #:upper16:__stack_sys_top

        @ Push the system load address on the stack
        stmfd   sp!, {r10}

        @ The stack for other processes running in system mode or user mode will be set up
        @ when respective process is created. 

        @ Add watermark patterns into the heap
        movw    r0, #:lower16:_heap_start
        movt    r0, #:upper16:_heap_start
        ldr     r1, =HEAP_WATERMARK
        movw    r2, #:lower16:_heap_end
        movt    r2, #:upper16:_heap_end
        sub     r2, r2, r0
        lsr     r2, r2, #2
        add     r2, r2, #1
        bl      memset32

        @ Zero out the bss, needed if we would like to run C code...
        movw    r0, #:lower16:_bss_start
        movt    r0, #:upper16:_bss_start
        mov     r1, #0
        movw    r2, #:lower16:_bss_end
        movt    r2, #:upper16:_bss_end
        sub     r2, r2, r0
        lsr     r2, r2, #2
        bl      memset32

        @ Enable the interface and functional clocks on GPIO_1
        movw    r0, #:lower16:CM_PER_BASE
        movt    r0, #:upper16:CM_PER_BASE
        movw    r1, #:lower16:CM_PER_FUNCT_INTRFC_CLCK_BM       @ functional and interface clocks
        movt    r1, #:upper16:CM_PER_FUNCT_INTRFC_CLCK_BM
        str     r1, [r0, #CM_PER_GPIO1_CLKCTRL]
1:
        ldr     r2, [r0, #CM_PER_GPIO1_CLKCTRL]
        cmp     r2, r1
        bne     1b

        @ Enable GPIOs used by the system
        bl      gpio_enable
        
        @ Turn on all user leds
        mov     r0, #GPIO1_ALL_USR_LEDS_BM
        bl      gpio_usrled_on

        @ Set up and enable the interrupt vector table
        bl      redirect_exception_vectors

        @ Set up the mux mapping needed for UART_0 (needed for serial communication)
        movw    r0, #:lower16:CTRL_MOD_BASE
        movt    r0, #:upper16:CTRL_MOD_BASE
        mov     r1, #0x0                                    @ mux selected
        orr     r1, r1, #CTRL_MODE_PULLUP_PULLDOWN_BM       @ Pullup/pulldown disabled
        
        @    - Set up the mux for Rx
        orr     r1, r1, #CTRL_MODE_RECEIVER_BM               @  Receiver enabled
        str     r2, [r0, #CTRL_MOD_OFFSET_CONF_UART0_RXD]
        
        @    - Set up the mux for Tx
        bic     r2, r2, #CTRL_MODE_RECEIVER_BM              @ Receiver disabled
        str     r2, [r0, #CTRL_MOD_OFFSET_CONF_UART0_TXD]
        
        @  Enable the interface and functional clocks on UART_0
        movw    r0, #:lower16:CM_WKUP_BASE
        movt    r0, #:upper16:CM_WKUP_BASE
        mov     r1, #WKUP_FUNCT_INTRFC_CLCK_BM
        str     r1, [r0, #CM_WKUP_UART0_CLKCTRL]
1:
        ldr     r2, [r0, #CM_WKUP_UART0_CLKCTRL]
        cmp     r2, r1
        bne     1b

        @ Set baud rate, data bits etc needed by the UART for serial communication on uart_0
        bl      uart_setup_serial_uart0
        
        @ Set up default IO configuartions.
        bl      setup_default_io_config








        /* At this point one usually do a lot of configurations to optimize the system, to increase the
         * performance and so on. However we relay on that the ROM code is setting up most of the thing 
         * needed in a somewhat proper way (see reference 1). If we end up in something that just must 
         * be handled this will be taken care of, otherwise we relay on that the ROM code setting up the
         * system in a somewhat proper way.
         */

        @ Perform other initializations...
        @ bl    init
        
         @ Branch off to main and do not return; cheerio and thanks for the fish...
        .section .text
        .code 32
        .align 2
        @ b     main


        /*********************************************************************** 
         * _deadloop
         *
         * Hang the system. If we end up here we do have a problem in the system.
         * (To be changed when we run several processes).
         *
         * C prototype: void _deadloop(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _deadloop
_deadloop:
        @ Turn off the led´s
        mov     r0, #GPIO1_ALL_USR_LEDS_BM
        bl      gpio_usrled_off
10:
        b       10b


        /*********************************************************************** 
         * redirect_exception_vectors
         *
         *  Redirect the exception vectors. Change the default addresses loaded 
         *  at the jump addresses.
         *
         * C prototype: void redirect_exception_vectors(void)
         **********************************************************************/
        .section .init
        .code 32
        .align 2
        .global redirect_exception_vectors
redirect_exception_vectors:

        addr        .req r0
        excHandl    .req r1

        /* clear the A bit - enable asynchronous aborts */
        /*mrs     r2, cpsr
        bic     r2, r2, #(1 << 8)
        msr     cpsr, r1*/

        /* Undefined Instruction */
        movw    addr, #:lower16:LOAD_ADDR_UNDEF_INSTR_EXC
        movt    addr, #:upper16:LOAD_ADDR_UNDEF_INSTR_EXC
        movw    excHandl, #:lower16:undef_instr_handl$
        movt    excHandl, #:upper16:undef_instr_handl$
        str     excHandl, [addr]

        /* Supervisor Call */
        movw    addr, #:lower16:LOAD_ADDR_SVC_EXC
        movt    addr, #:upper16:LOAD_ADDR_SVC_EXC
        movw    excHandl, #:lower16:swc_handl$
        movt    excHandl, #:upper16:swc_handl$
        str     excHandl, [addr]

        /* Prefetch Abort */
        movw    addr, #:lower16:LOAD_ADDR_PREF_ABRT_EXC
        movt    addr, #:upper16:LOAD_ADDR_PREF_ABRT_EXC
        movw    excHandl, #:lower16:prefetch_abort_handl$
        movt    excHandl, #:upper16:prefetch_abort_handl$
        str     excHandl, [addr]

        /* Data Abort */
        movw    addr, #:lower16:LOAD_ADDR_DATA_ABRT_EXC
        movt    addr, #:upper16:LOAD_ADDR_DATA_ABRT_EXC
        movw    excHandl, #:lower16:data_abort_handl$
        movt    excHandl, #:upper16:data_abort_handl$
        str     excHandl, [addr]

        /* IRQ Interrupt */
        movw    addr, #:lower16:LOAD_ADDR_IRQ_INTR
        movt    addr, #:upper16:LOAD_ADDR_IRQ_INTR
        movw    excHandl, #:lower16:irq_handl$
        movt    excHandl, #:upper16:irq_handl$
        str     excHandl, [addr]

        /* FIQ Interrupt */
        movw    addr, #:lower16:LOAD_ADDR_FIQ_INTR
        movt    addr, #:upper16:LOAD_ADDR_FIQ_INTR
        movw    excHandl, #:lower16:fiq_handl$
        movt    excHandl, #:upper16:fiq_handl$
        str     excHandl, [addr]

        mov     pc, lr
        .unreq  addr
        .unreq  excHandl


        @ Handle the below in other files or an other way... TBC
undef_instr_handl$:
swc_handl$:
prefetch_abort_handl$:
data_abort_handl$:
irq_handl$:
fiq_handl$:
        b       _deadloop












