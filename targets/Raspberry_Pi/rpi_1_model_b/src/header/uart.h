/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  uart.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Updated and copied to bOS
 *      ver 5       Updated function names
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init(void);

// Do not use this functions directly, use the io functions in io.h instead...
void uart_putc(const char ch);
void uart_puts(const char *str, const uint32_t len);


#endif /* UART_H_ */