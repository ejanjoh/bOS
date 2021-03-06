/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-07
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  start.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *      ver 5       _deadloop updated
 *      ver 7       Enable interrupts
 *      ver 10      Updated _deadloop.
 *                  Added "function" _get_stack_ptr.
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"


        .equ    DEAD_LOOP_CNT,  0x800000


/* @ address 0x00000000
 *
 * The interrupt vector table will be copied to this address (0x00000000)
 * during the boot of the kernel, see below. The startup of the MPU is initiated
 * by the GPU and the kernel is loaded at address 0x8000 per default. But we 
 * need to put the interrupt vector table at a proper place to make sure that 
 * the exceptions works as intended.
 *
 * However, it´s possible to change this and place the interrupt vector table 
 * and the start-up of the system at address 0x00000000 by changing in the file 
 * config.txt that configure how the GPU initiate the system (not done here). An
 * alternative to this is to use the high address interrupt vector table.
 *
 *
 *          exception                 processor mode    address
 *          ______________________________________________________
 *          reset                     supervisor        0x00000000
 *          undefined instruction     undefined         0x00000004
 *          software interrupt        supervisor        0x00000008
 *          prefetch abort            abort             0x0000000C
 *          data abort                abort             0x00000010
 *          Reserved for future expansion of the ARM architecture
 *          interrupt (irq)           irq               0x00000018
 *          fast interrupt (fiq)      fiq               0x0000001C
 */


        /*********************************************************************** 
         * _start
         *
         * Global start of the software system (non ROM system). 
         *
         * @ address 0x00008000: start of .init section and at boot this is the 
         * first code running in bOS, i.e. bOS system start.
         *
         * C prototype: N/A 
         **********************************************************************/
        .section .init
        .code 32
        .align 2
        .global _start
_start:

        ldr     pc, reset_jmp$
        ldr     pc, undef_jmp$
        ldr     pc, swi_jmp$
        ldr     pc, pref_jmp$
        ldr     pc, data_jmp$
        ldr     pc, resv_jmp$
        ldr     pc, irq_jmp$
        ldr     pc, fiq_jmp$

@ Jump table for exceptions
reset_jmp$:     .word   reset_handl$
undef_jmp$:     .word   _undef_instr_handl
swi_jmp$:       .word   _swi_handl
pref_jmp$:      .word   _prefetch_abort_handl
data_jmp$:      .word   _data_abort_handl
resv_jmp$:      .word   reserved$               @ major problem if we end up here...
irq_jmp$:       .word   _irq_handl
fiq_jmp$:       .word   _fiq_handl

reset_handl$:
        @ Copy the interrupt vector table to its position
        mov     r0, #INTERRUPT_VECTOR_TABLE_ADDR
        mov     r1, #SYSTEM_START_ADDR
        ldm     r1!,{r2 - r9}
        stm     r0!,{r2 - r9}
        ldm     r1, {r2 - r9}
        stm     r0, {r2 - r9}

        @ Use the area below SYSTEM_START_ADDR as a temporary stack while the stack region in memory
        @ are filled up with watermarks.
        mov     sp, #SYSTEM_START_ADDR          @ Assign a temporary stack to the
        sub     sp, sp, #4                      @ current stack pointer (supervisor mode)

        ldr     r0, =_stack_start               @ start mark at this address
        ldr     r1, =STACK_WATERMARK_PAT        @ pattern to be set
        ldr     r2, =_stack_end
        sub     r2, r2, r0
        add     r2, r2, #4
        lsr     r2, r2, #2                      @ = # of words to be set to watermark pattern
        bl      memset32
        
        @ Zero out the .bss sector
        ldr     r0, =_bss_start
        mov     r1, #0
        ldr     r2, =_bss_end
        sub     r2, r2, r0
        add     r2, r2, #4
        lsr     r2, r2, #2
        bl      memset32

        @ Set up the stack at supervisor mode (exception), the current mode...
        ldr     sp, =__stack_exc_svc_top

        @ Set up the stack at abort mode (exception)
        mov     r0, #ABT32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1                      @ system mode changed to abort mode
        ldr     sp, =__stack_exc_abt_top

        @ Set up the stack at undefined mode (exception)
        mov     r0, #UND32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1                      @ system mode changed to undefined mode
        ldr     sp, =__stack_exc_und_top

        @ Set up the stack at interrupt mode (exception)
        mov     r0, #IRQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1                      @ system mode changed to interrupt mode
        ldr     sp, =__stack_exc_irq_top

        @ Set up the stack at fast interrupt mode (exception)
        mov     r0, #FIQ32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1                      @ system mode changed to fast interrupt mode
        ldr     sp, =__stack_exc_fiq_top

        @ Set up the stack at system mode (kernel operating mode)
        mov     r0, #SYS32_MODE
        mrs     r1, cpsr
        bic     r1, r1, #CPSR_SYSTEM_MODE_BM
        orr     r1, r1, r0
        msr     cpsr_c, r1                      @ system mode changed to system mode
        ldr     sp, =__stack_sys_top

        @ The stack for other processes running in system mode or user mode will be set up
        @ when respective process is created. 

        @ Add watermark patterns into the heap
        ldr     r0, =_heap_start
        ldr     r1, =HEAP_WATERMARK_PAT
        ldr     r2, =_heap_end
        sub     r2, r2, r0
        lsr     r2, r2, #2
        add     r2, r2, #1
        bl      memset32

        @ Turn on the ACT-led; this to indicate a system started and that all is ok so far
        mov     r0, #GPIO_GPFSEL1_OFFSET
        mov     r1, #GPIO_GPFSEL_GPIO_X6_TRIPLET_MASK
        mov     r2, #GPIO_16_ACT_LED_MASK
        bl      gpio_funcsel                    @ configure the GPIO pin used for the ACT led.
        mov     r0, #GPIO_PIN_16_MASK
        bl      gpio_output_high                @ turn on the ACT led
        
        @ Set up GPIO 14 and 15 to handle UART TX and RX respectively
        bl      gpio_setup_uart
        
        @ Initialize the UART
        bl      uart_init
        
        @ Set up default IO configuartions.
        bl      setup_default_io_config


        /* At this point one usually do a lot of configurations to optimize the system, to increase the
         * performance and so on. However the information from Broadcom what happens when the GPU
         * setting up the system is to my knowledge not public available, nor is documentation apart
         * from BCM2835 ARM Peripherals availible. ARM provide quite a lot of information and one could
         * of cause take a look into a proper Linux distribution to find out details. Usually one invest
         * quite some time in writing this, through not done here because this is a operating system 
         * project and not any intention to write a proper boot... (take a look att 
         * //github.com/ejanjoh/BeagleBone-Black to get an idea of what is needed, and that is just pure
         * basics... A lot more needs to be done.)
         *
         * If we end up in something that just must be handled this will be taken care of, otherwise we
         * relay on that the GPU setting up the system in a somewhat proper way.
         */


        @ Perform other initializations...
        bl      init
        
        @ Disturb the timer interrupt coming from the GPU so they not affect the MPU context
        ldr     r0, =SYSTEM_TIMER_BASE
        ldr     r1, [r0, #SYSTEM_TIMER_CLO_OFFSET]
        add     r1, r1, #SYS_TIMER_INTR_INTERVAL_PAT
        str     r1, [r0, #SYSTEM_TIMER_C0_OFFSET]
        str     r1, [r0, #SYSTEM_TIMER_C2_OFFSET]
        
        @ Clear timer channel (M0 + M2)
        ldr     r1, =SYS_TIMER_CS_M0_MASK | SYS_TIMER_CS_M2_MASK
        str     r1, [r0, #SYSTEM_TIMER_CS_OFFSET]
        
        @ Enable interrupt for handling timers, UARTs etc...
        mrs     r4, cpsr
        bic     r4, r4, #CPSR_IRQ_BM
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
        orr     r4, r4, #(CPSR_IRQ_BM | CPSR_FIQ_BM)
        msr     cpsr_c, r4

        @ Turn the ACT led on and off to indicate an error or issue that hang the system (e.g. an assert)
10:
        mov     r0, #GPIO_PIN_16_MASK
        bl      gpio_output_low                 @ turn off the ACT led
        mov     r1, #0
11:
        add     r1, r1, #1
        cmp     r1, #DEAD_LOOP_CNT
        bne     11b
        
        mov     r0, #GPIO_PIN_16_MASK
        bl      gpio_output_high
        mov     r1, #0
12:
        add     r1, r1, #1
        cmp     r1, #DEAD_LOOP_CNT
        bne     12b

        b       10b

@ We have major problems...
reserved$:
        ldr     r4, =_puts 
        ldr     r4, [r4]
        ldr     r0, =reserved_print_out$
        mov     r1, #100
        blx     r4
        b       _deadloop
reserved_print_out$:
.asciz "RESERVED in IVT (MAJOR PROBLEMS)\n"
.align 2


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









