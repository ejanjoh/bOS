/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-11-08
 *      Updated:    2015-05-23
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  uart_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File copied and updated
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"



        /*********************************************************************** 
         * uart_getc
         *
         * Get a char on UART0/serial interface. Note, this "function" will hang
         * until a char is recived on the UART0.
         *
         * out:     - char from UART0
         *
         * C prototype: char uart_getc(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global uart_getc
uart_getc:
        movw    r1, #:lower16:UART0_BASE
        movt    r1, #:upper16:UART0_BASE
        
        @ Wait unil there is a char to be read on the UART0.
1:
        ldrh    r2, [r1, #UART_LSR_UART_OFFSET]
        and     r2, r2, #UART_LSR_UART_RXFIFOE_MASK         @ are there any chars?
        cmp     r2, #0
        beq     1b
2:
        ldrh    r0, [r1, #UART_RHR_OFFSET]
        and     r0, r0, #0xFF
        mov     pc, lr


        /*********************************************************************** 
         * uart_putc
         *
         * Transmit or put a char on UART0/serial interface (according to the 
         * BBB System Reference Manual the serial pins/serial debug port are 
         * connected to the UART0)
         *
         * in: r0 - char to be placed on UART0
         *
         * C prototype: void uart_putc(const char c) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global uart_putc
uart_putc:

        and     r0, r0, #0xFF
        movw    r1, #:lower16:UART0_BASE
        movt    r1, #:upper16:UART0_BASE

        /* Secure that UART is ready to tramsmit a char */
1:
        ldrh    r2, [r1, #UART_LSR_UART_OFFSET]
        and     r2, r2, #UART_LSR_UART_TXFIFOE_MASK         @ ready to transmit?
        cmp     r2, #0
        beq     1b
2:
        strh    r0, [r1, #UART_THR_OFFSET]
        mov     pc, lr


        /*********************************************************************** 
         * uart_puts
         *
         * Transmit or put a null terminated string on UART0/serial interface.
         * If there is a null termination before the len:th char it will stop 
         * putting chars on the serial interface. If not, it will stop at len:th
         * char.
         *
         * in: r0 - a pointer to a null terminated string
         *     r1 - the maximum lenght of the string
         *
         * C prototype: void uart_puts(char *str, uint32_t len) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global uart_puts
uart_puts:
        stmfd   sp!, {r4, lr}
        mov     r2, r0
        ldrb    r0, [r2]
1:
        and     r0, r0, #0xFF
        cmp     r0, #0x0
        beq     10f
        cmp     r1, #0x0
        beq     10f

        stmfd   sp!, {r0-r3}
        bl      uart_putc
        ldmfd   sp!, {r0-r3}

        sub     r1, r1, #1
        ldrb    r0, [r2, #1]!
        b       1b

10:
        ldmfd   sp!, {r4, pc}


        /*********************************************************************** 
         *  uart_setup_serial_uart0
         *
         *  Setup a serial interface on UART0 as:
         *      - 115200 baud
         *      - 8 data bits
         *      - 1 stop bit
         *      - no parity
         *      - no interrupt
         *
         * C prototype: void uart_setup_serial_uart0(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global uart_setup_serial_uart0
uart_setup_serial_uart0:
        stmfd   sp!, {r7-r9}

        @ --- Software reset - UART ---
        movw    r0, #:lower16:UART0_BASE
        movt    r0, #:upper16:UART0_BASE
1:
        add     r1, r0, #UART_SYSC_OFFSET                   @ ldrh can only handle 5 bit offset
        ldrh    r2, [r1]
        orr     r2, r2, #UART_SYSC_SOFTRESET_MASK
        strh    r2, [r1]

        add     r1, r0, #UART_SYSS_OFFSET
        @ loop until reset is performed
2:
        ldrh    r2, [r1]
        and     r2, r2, #UART_SYSS_RESETDONE_MASK
        cmp     r2, #UART_SYSS_RESETDONE_MASK
        bne     2b  

        @ --- FIFOs and DMA Settings ---
        movw    r0, #:lower16:UART0_BASE
        movt    r0, #:upper16:UART0_BASE
1:
        ldrh    r7, [r0, #UART_LCR_OFFSET]                  @ r7 = UART_LCR, to be used later on...
        mov     r2, #UART_REG_CONFIG_MODE_B_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
2:
        ldrh    r8, [r0, #UART_EFR_OFFSET]                  @ r8 = UART_EFR, to be used later on...
        orr     r2, r8, #UART_EFR_ENHANCED_EN_MASK
        strh    r2, [r0, #UART_EFR_OFFSET]
3:
        mov     r2, #UART_REG_CONFIG_MODE_A_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
4:
        add     r1, r0, #UART_MCR_OFFSET
        ldrh    r9, [r1]                                    @ r9 = UART_MCR, to be used later on...
        orr     r2, r9, #UART_MCR_TCR_TLR_MASK
        strh    r2, [r1]
5:
        mov     r2, #UART_NO_DMA_NOR_FIFO_MASK
        strh    r2, [r0, #UART_FCR_OFFSET]
6:
        mov     r2, #UART_REG_CONFIG_MODE_B_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
7:
        @ Skipped, no FIFO in use
8:
        @ Skipped, No FIFO nor DMA in use
9:
        and     r8, r8, #UART_EFR_ENHANCED_EN_MASK
        ldrh    r2, [r0, #UART_EFR_OFFSET]
        bic     r2, r2, #UART_EFR_ENHANCED_EN_MASK
        orr     r2, r2, r8
        strh    r2, [r0, #UART_EFR_OFFSET]
10:
        mov     r2, #UART_REG_CONFIG_MODE_A_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
11:
        and     r9, r9, #UART_MCR_TCR_TLR_MASK
        ldrh    r2, [r0, #UART_MCR_OFFSET]
        bic     r2, r2, #UART_MCR_TCR_TLR_MASK
        orr     r2, r2, r9
        str     r2, [r0, #UART_MCR_OFFSET]
12:
        strh    r7, [r0, #UART_LCR_OFFSET]

        @ --- UART serial settings ---
        movw    r0, #:lower16:UART0_BASE
        movt    r0, #:upper16:UART0_BASE
        add     r1, r0, #UART_MDR1_OFFSET                   @ ldrh can only handle 5 bit offset
1:
        ldrh    r2, [r1]
        orr     r2, r2, #UART_MDR1_MODE_SELECT_MASK
        strh    r2, [r1]
2:
        mov     r2, #UART_REG_CONFIG_MODE_B_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
3:
        ldrh    r7, [r0, #UART_EFR_OFFSET]                  @ r7 = UART_EFR, to be used later on...
        mov     r2, r7
        orr     r2, r2, #UART_EFR_ENHANCED_EN_MASK
        strh    r2, [r0, #UART_EFR_OFFSET]
4:
        mov     r2, #UART_REGISTER_OPERATIONAL_MODE_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
5:
        mov     r2, #UART_IER_CLEAR_MASK
        strh    r2, [r0, #UART_IER_UART_OFFSET]
6:
        mov     r2, #UART_REG_CONFIG_MODE_B_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
7:
        mov     r2, #UART_DLL_CLOCK_LSB_MASK
        strh    r2, [r0, #UART_DLL_OFFSET]                  @ DLL-part of 115200 baud
        mov     r2, #UART_DLH_CLOCK_MSB_MASK
        strh    r2, [r0, #UART_DLH_OFFSET]                  @ DLH-part of 115200 baud
8:
        mov     r2, #UART_REGISTER_OPERATIONAL_MODE_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
9:
        mov     r2, #UART_IER_INTERRUPT_DISABLED_MASK       @ Interrupt isn´t enabled
        strh    r2, [r0, #UART_IER_UART_OFFSET]
10:
        mov     r2, #UART_REG_CONFIG_MODE_B_MASK
        strh    r2, [r0, #UART_LCR_OFFSET]
11:
        ldrh    r2, [r0, #UART_EFR_OFFSET]
        bic     r2, #UART_EFR_ENHANCED_EN_MASK
        and     r7, r7, #UART_EFR_ENHANCED_EN_MASK
        orr     r2, r2, r7
        strh    r2, [r0, #UART_EFR_OFFSET]                  @ Restore UART_EFR
12:
        mov     r2, #UART_LCR_8_BIT_CHAR_LENGTH_MASK
        add     r2, r2, #UART_LCR_1_STOP_BIT_MASK
                                                            @ & no parity
        strh    r2, [r0, #UART_LCR_OFFSET]
13:
        ldrh    r2, [r1]
        bic     r2, #UART_MDR1_MODE_SELECT_MASK             @ UART 16x mode
        strh    r2, [r1]

        ldmfd   sp!, {r7-r9}
        mov     pc, lr




