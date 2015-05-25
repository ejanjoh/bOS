/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  uart.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       Updated and copied to bOS
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#ifndef UART_H_
#define UART_H_

#include <stdint.h>


// Do not use this functions directly, use the io functions in io.h instead...
char uart_getc(void);
void uart_putc(const char ch);
void uart_puts(const char *str, const uint32_t len);


#endif /* UART_H_ */