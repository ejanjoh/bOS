/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-12
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  uart.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Updated and copied to bOS
 *      ver 5       Updated function names
 *                  Adjusted end of line format
 *      ver 9       Protected the UART0 used for serial communication with a 
 *                  mutex.
 *      ver 10      Added functionality for read UART input.
 *                  Changed handling of e.o.l. chars
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stddef.h>
#include "uart.h"
#include "hardware_system.h"
#include "semaphore.h"
#include "assert.h"



/* The RPI has two UARTs, one mini-UART and one PL011 UART; this is the initialization
 * of the later UART. */
void uart_init(void)
{
    //volatile uint32_t *pReg = NULL;

    // Disable the UART (and every thing else controlled by the UART control register)
    SET32(UART_BASE + UART_CR_OFFSET, UART_CR_DISABLE_UART_PAT);

    // Clear potential pending interrupts and disable all interrupts.
    SET32(UART_BASE + UART_ICR_OFFSET, UART_ICR_CLEAR_INTERRUPT_PAT);
    SET32(UART_BASE + UART_IMSC_OFFSET, UART_IMSC_CLEAR_INTERRUPT_MASK_PAT);

    /* Set the integer and fractional baud rate divisors:
     * baud = 115200
     * UART_CLK = 3 MHz (*)
     *
     * According to reference:
     * Baud Rate Divisor (BRD) = F_uart_clk/(16 * baud_rate) = BRD_int + BRD_frac
     * Fractional part m = int((BRD_frac * 64) + 0,5)
     *
     * BRD = 3000000/(16 * 115200) = 1,62760 ==>
     * BRD_int = 1
     * BRD_frac = 0,62760
     * m = int((0,62760 * 64) + 0,5) = int(40,666) = 40 = 0x28
     *
     * (*) The frequency of the UART clock is not found in any of the reference
     * documents. However quite some people has investigated this and the
     * frequency 3 MHz is often used by others (e.g. take a look at some of the
     * Linux distributions available) */
    SET32(UART_BASE + UART_IBRD_OFFSET, UART_IBRD_PAT);
    SET32(UART_BASE + UART_FBRD_OFFSET, UART_FBRD_PAT);

    /* - Disable stick parity
     * - Enable 8 bits data per frame
     * - Enable FIFO
     * - Disable two stop bits
     * - Disable parity */
    SET32(UART_BASE + UART_LCRH_OFFSET, UART_LCRH_ENABLE_8_BIT_DATA_MASK);      // FIFO disabled
    //SET32(UART_BASE + UART_LCRH_OFFSET, UART_LCRH_ENABLE_8_BIT_DATA_MASK | UART_LCRH_ENABLE_FIFO_MASK);

    // Set FIFO interrupt trigger level 1/8 full
    SET32(UART_BASE + UART_IFLS_OFFSET, UART_IFLS_FIFO_LEVEL_SELECT_PAT);

    // Enable the interrupts
    SET32(UART_BASE + UART_IMSC_OFFSET, UART_IMSC_SET_RX_MASK);                 // RX interrupt mask only

    // Enable RX, TX and the UART
    SET32(UART_BASE + UART_CR_OFFSET, UART_CR_RX_ENABLE_MASK | UART_CR_TX_ENABLE_MASK | UART_CR_UART_ENABLE_MASK);

    return;
}


// Get the first character in the PL011 UARTs FIFO
char _uart_getc(void)
{
    // If the RX FIFO is empty, wait...
    while (GET32(UART_BASE + UART_FR_OFFSET) & UART_FR_RX_FIFO_EMPTY_MASK) ;

    return (char) GET32(UART_BASE + UART_DR_OFFSET);
}


// Put a character in the PL011 UARTs FIFO for transmit
void _uart_putc(const char ch)
{
    // If the transmit FIFO is full, wait...
    while (GET32(UART_BASE + UART_FR_OFFSET) & UART_FR_TRANSMIT_FIFO_FULL_MASK);
    
    if ('\n' != ch) {
        SET32(UART_BASE +  UART_DR_OFFSET, ch);
    }
    else {
        SET32(UART_BASE +  UART_DR_OFFSET, '\r');
        // If the transmit FIFO is full, wait...
        while (GET32(UART_BASE + UART_FR_OFFSET) & UART_FR_TRANSMIT_FIFO_FULL_MASK);
        SET32(UART_BASE +  UART_DR_OFFSET, '\n');
    }

    return;
}


// Put a null terminated string at the PL011 UARTs FIFO for transmit
void _uart_puts(const char *str, const uint32_t len)
{
    char *p = (char *) str;
    
    ASSERT(sizeof(p) < len);
    ASSERT(NULL != p);
    ASSERT('\0' != *p);
    
       
    while ('\0' != *p) {
        // If the transmit FIFO is full, wait...
        while (GET32(UART_BASE + UART_FR_OFFSET) & UART_FR_TRANSMIT_FIFO_FULL_MASK);

        if ('\n' != *p) {
            SET32(UART_BASE +  UART_DR_OFFSET, *p++);
        }
        else {
            p++;
            SET32(UART_BASE +  UART_DR_OFFSET, '\r');
            // If the transmit FIFO is full, wait...
            while (GET32(UART_BASE + UART_FR_OFFSET) & UART_FR_TRANSMIT_FIFO_FULL_MASK);
            SET32(UART_BASE +  UART_DR_OFFSET, '\n');
        }
    } // while (...)

    return;
}


