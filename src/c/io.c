/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-17
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  io.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Created
 *      ver 5       Changed some function names to more common ones, even if
 *                  the interface differs somewhat.
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include "uart.h"



void (* putc)(const char ch) = NULL;
void (* puts)(const char *str, const uint32_t len) = NULL;


void setup_default_io_config(void)
{
    /* This shall be changed when we have the process framework, inclusive process communication,
       up and running. A more proper io solution with connection to io-streams. But until then
       this is an acceptable solution. */
    
    putc = uart_putc;
    puts = uart_puts;
    
    puts("Default out configured...\n", 100);
    putc('O');
    putc('k');

    return;
}





