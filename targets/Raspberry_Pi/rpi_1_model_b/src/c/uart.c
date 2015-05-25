/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-12
 *      Updated:    
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
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stddef.h>
#include "uart.h"
#include "hardware_system.h"


/* The RPI has two UARTs, one mini-UART and one PL011 UART; this is the initialization
 * of the later UART. */
void uart_init(void)
{
    volatile uint32_t *pReg = NULL;

    // Disable the UART (and every thing else controlled by the UART control register)
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_CR);
    *pReg = UART_CR_DISABLE_UART;

    // Clear potential pending interrupts and disable all interrupts.
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_ICR);
    *pReg = UART_ICR_CLEAR_INTERRUPT;
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_IMSC);
    *pReg = UART_IMSC_CLEAR_INTERRUPT_MASK;

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
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_IBRD);
    *pReg = UART_IBRD;
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_FBRD);
    *pReg = UART_FBRD;

    /* - Disable stick parity
     * - Enable 8 bits data per frame
     * - Enable FIFO
     * - Disable two stop bits
     * - Disable parity */
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_LCRH);
    //*pReg = UART_LCRH_ENABLE_8_BIT_DATA | UART_LCRH_ENABLE_FIFO;
    *pReg = UART_LCRH_ENABLE_8_BIT_DATA;                  // FIFO disabled

    // Set FIFO interrupt trigger level 1/8 full
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_IFLS);
    *pReg = UART_IFLS_FIFO_LEVEL_SELECT;

    // Enable the interrupts
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_IMSC);
    *pReg = UART_IMSC_SET_RX;           // RX interrupt mask only

    // Enable RX, TX and the UART
    pReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_CR);
    *pReg = UART_CR_RX_ENABLE | UART_CR_TX_ENABLE | UART_CR_UART_ENABLE;

    return;
}

// Get the first character in the PL011 UARTs FIFO
char uart_getc(void)
{
    volatile uint32_t *pFlagReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_FR);
    volatile uint32_t *pDataReg = (volatile uint32_t *) (UART_BASE +  UART_OFFSET_DR);

    // If the RX FIFO is empty, wait...
    while ((*pFlagReg) & UART_FR_RX_FIFO_EMPTY_BM);

    return (char) *pDataReg;
}

// Put a character in the PL011 UARTs FIFO for transmit
void uart_putc(const char ch)
{
    volatile uint32_t *pFlagReg = (volatile uint32_t *) (UART_BASE + UART_OFFSET_FR);
    volatile uint32_t *pDataReg = (volatile uint32_t *) (UART_BASE +  UART_OFFSET_DR);

    /* If the transmit FIFO is full, wait... */
    while ( (*pFlagReg) & UART_FR_TRANSMIT_FIFO_FULL_BM );

    *pDataReg = (const uint32_t) ch;
    return;
}


// Put a null terminated string at the PL011 UARTs FIFO for transmit
void uart_puts(const char *str, const uint32_t len)
{
    const char * pStr = str;
    uint32_t n = 0;

    while (('\0' != *pStr) && (n < len)) {
        uart_putc(*pStr);                    // remove this function call to improve performance
        pStr++;
        n++;
    }

    return;
}