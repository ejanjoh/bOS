/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-19
 *      Updated:    2016-05-19
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  start.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File created
 *      ver 7       Added support to handle context switching
 *      ver 10      Corrected an error on UART0 mux mapping
 *      ver 12      Adopted changes to get level DDR3 SDRAM up and running.
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"


        .equ    DEAD_LOOP_CNT,  0x500000


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
        ldr     r1, =STACK_WATERMARK_PAT
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
        bic     r1, r1, #CPSR_SYSTEM_MODE_MASK
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to undefined mode
        movw    sp, #:lower16:__stack_und_top
        movt    sp, #:upper16:__stack_und_top

        @ Change to abort mode
        mov     r0, #ABT32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_MASK
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to abort mode
        movw    sp, #:lower16:__stack_abt_top
        movt    sp, #:upper16:__stack_abt_top

        @ Change to irq mode
        mov     r0, #IRQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_MASK
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to irq mode
        movw    sp, #:lower16:__stack_irq_top
        movt    sp, #:upper16:__stack_irq_top

        @ Change to fiq mode
        mov     r0, #FIQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_MASK
        orr     r1, r1, r0
        msr     cpsr_c, r1

        @ Assign a stack pointer to fiq mode
        movw    sp, #:lower16:__stack_fiq_top
        movt    sp, #:upper16:__stack_fiq_top

        @ Change to system mode
        mov     r0, #SYS32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_MASK
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
        movw    r1, #:lower16:HEAP_WATERMARK_PAT
        movt    r1, #:upper16:HEAP_WATERMARK_PAT
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
        
        @ Setup and initiate the PLLs and clocks needed by the system.
        bl      clock_setup

        @ Enable the interface and functional clocks on GPIO_1
        @bl      clock_enable_gpio1     /* moved to clock_setup */

        @ Enable GPIOs used by the system
        bl      gpio_enable
        
        @ Turn on all user leds
        mov     r0, #GPIO1_ALL_USR_LEDS_MASK
        bl      gpio_usrled_on

        @ Set up and enable the new interrupt vector table
        bl      redirect_exception_vectors
        
        @ Set up the mux mapping needed for UART0 (needed for serial communication)
        movw    r0, #:lower16:CTRL_MODE_BASE
        movt    r0, #:upper16:CTRL_MODE_BASE
        mov     r1, #CTRL_MODE_UART0_RX_TX_MUXMODE_PAT
        orr     r1, #CTRL_MODE_PULLUDEN_MASK                @ Pullup/pulldown disabled
        orr     r1, #CTRL_MODE_PULLTYPESEL_MASK
        
        @    - Setup the mux for UART0 Rx serial line
        orr     r1, #CTRL_MODE_RXACTIVE_MASK                @ Receiver enabled
        str     r1, [r0, #CTRL_MODE_CONF_UART0_RXD_OFFSET]

        @    - Setup the mux for UART0 Tx serial line
        bic     r1, r1, #CTRL_MODE_RXACTIVE_MASK            @ Receiver disabled
        str     r1, [r0, #CTRL_MODE_CONF_UART0_TXD_OFFSET]

        @  Enable the interface and functional clocks on UART0
        @bl      clock_enable_uart0     /* moved to clock_setup */

        @ Set baud rate, data bits etc needed by the UART for serial communication on uart_0
        bl      uart_setup_serial_uart0
        
        @ Set up default IO configuartions.
        bl      setup_default_io_config
        
        @ Set up timer 2 (used for timer interrupt to the context switch)
        @bl      clock_enable_timer2        /* moved to clock_setup */
        
        @ Set up and configure the (L3) EMIF - DDR3 SDRAM
        bl      Config_EMIF_DDR3_SDRAM

        @ Test the SDRAM - performs a linear test on the SDRAM
        @bl      TestSDRAM

        /* At this point one usually do a lot of configurations to optimize the system, to increase the
         * performance and so on. However we relay on that the ROM code is setting up most of the thing 
         * needed in a somewhat proper way (see reference 1). If we end up in something that just must 
         * be handled this will be taken care of, otherwise we relay on that the ROM code setting up the
         * system in a somewhat proper way.
         */

        @ Copy .text, .bss, .rodata, .data sections to the L3 SDRAM
        movw    r0, #:lower16:_CPY_TO_L3_SDRAM_START
        movt    r0, #:upper16:_CPY_TO_L3_SDRAM_START
        mov     r1, #DDR3_SDRAM_START_ADDR        
        movw    r2, #:lower16:_CPY_TO_L3_SDRAM_END
        movt    r2, #:upper16:_CPY_TO_L3_SDRAM_END
        sub     r2, r2, r0
        lsr     r2, r2, #2
        bl      memcopy32

        @ Test that the first 100 bytes doesn't differ...
        movw    r0, #:lower16:_CPY_TO_L3_SDRAM_START
        movt    r0, #:upper16:_CPY_TO_L3_SDRAM_START
        mov     r1, #DDR3_SDRAM_START_ADDR
        mov     r2, #100
        bl      memcmp
        cmp     r0, #0
        beq     10f
        movw    r0, #:lower16:TEXT_1_SDRAM_COPY_ERROR
        movt    r0, #:upper16:TEXT_1_SDRAM_COPY_ERROR
        mov     r1, #80
        bl      _uart_puts
        b       _deadloop       @ no end...
10:
        movw    r0, #:lower16:TEXT_2_SDRAM_COPY_OK
        movt    r0, #:upper16:TEXT_2_SDRAM_COPY_OK
        mov     r1, #80
        bl      _uart_puts

        @ Perform other initializations...
        bl      init

        @ Calculate the offset to L3_START_ADDRESS on L3 SDRAM
        movw    r0, #:lower16:L3_START_ADDRESS
        movt    r0, #:upper16:L3_START_ADDRESS
        movw    r1, #:lower16:_CPY_TO_L3_SDRAM_START
        movt    r1, #:upper16:_CPY_TO_L3_SDRAM_START
        sub     r0, r0, r1

        /* Move execution to L3_START_ADDRESS to L3 RAM */
        add     r0, r0, #DDR3_SDRAM_START_ADDR
        mov     pc, r0

        /*********************************************************************** 
         * .section .text
         *
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global L3_START_ADDRESS
L3_START_ADDRESS:

        @ We now run in DDR3 SDRAM address range
        add     r10, pc, #-0x8                          @ r10 = current address

        @ Notify the world about the move of execution point...
        movw    r0, #:lower16:TEXT_3_EXECUTION_MOVE
        movt    r0, #:upper16:TEXT_3_EXECUTION_MOVE
        mov     r1, #80
        bl      _uart_puts
        mov     r0, r10
        bl      _uart_put_hex32
        mov     r0, #'\n'
        bl      _uart_putc
        mov     r0, #'\r'
        bl      _uart_putc

        @ Enable IRQ on MPU
        mrs     r4, cpsr
        bic     r4, r4, #CPSR_IRQ_MASK
        msr     cpsr_c, r4

        @ Branch off to main and do not return; cheerio and thanks for the fish...
        b       main


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
        @ Disable IRQ/FIQ on MPU
        mrs     r4, cpsr
        orr     r4, r4, #(CPSR_IRQ_MASK | CPSR_FIQ_MASK)
        msr     cpsr_c, r4

        @ Turn the usr leds on and off to indicate an error or issue that hang the system (e.g. an assert)
10:
        mov     r0, #GPIO1_ALL_USR_LEDS_MASK
        bl      gpio_usrled_off
        mov     r1, #0
11:
        add     r1, r1, #1
        cmp     r1, #DEAD_LOOP_CNT
        bne     11b
        
        mov     r0, #GPIO1_ALL_USR_LEDS_MASK
        bl      gpio_usrled_on
        mov     r1, #0
12:
        add     r1, r1, #1
        cmp     r1, #DEAD_LOOP_CNT
        bne     12b

        b       10b


        /*********************************************************************** 
         * _get_stack_ptr(void)
         *
         * Get the current stack pointer
         *
         * C prototype: uint32_t *_get_stack_ptr(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global _get_stack_ptr
_get_stack_ptr:
        mov     r0, sp
        mov     pc, lr


        /*********************************************************************** 
         * .section .rodata
         *
         **********************************************************************/
        .section .rodata
        .align 2
TEXT_1_SDRAM_COPY_ERROR:
        .asciz "Error - the copied value to DDR3 RAM is not identical to original...\n\r"
        .align 2
TEXT_2_SDRAM_COPY_OK:
        .asciz ".text, .bss, .rodata and .data sections successfully copied to the DDR3 SDRAM\n\r"
        .align 2
TEXT_3_EXECUTION_MOVE:
        .asciz "Point of execution moved to: "
        .align 2




