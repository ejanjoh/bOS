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
 *      ver 6       Added formated output by printf(...)
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "uart.h"
#include "log.h"
#include "assert.h"




void (* putc)(const char ch) = NULL;
void (* puts)(const char *str, const uint32_t len) = NULL;


void setup_default_io_config(void)
{
    /* This shall be changed when we have the process framework, inclusive process communication,
       up and running. A more proper io solution with connection to io-streams. But until then
       this is an acceptable solution. */
    
    putc = uart_putc;
    puts = uart_puts;
    
    puts("Default out configured...\r\n", 100);

    return;
}


int32_t printf(const uint32_t len, const char *format, ...)
{
    va_list ap;
    char str[len];      /* to be changed to malloc when available... */
    char *pStr = str;
    uint32_t totLen = 0;
    
    LOG_WARNING_RETURN_NEGATIVE( !(0 == len) );
    LOG_WARNING_RETURN_NEGATIVE( !(NULL == format) );
	va_start(ap, format);
    
    
    while ('\0' != *format && totLen < len) {
    
        if ('%' != *format) {
            *pStr++ = *format++;
            totLen++;
        }
        else {
            format++;
            switch(*format++) {
                case '%':
                    *pStr++ = *format++;
                    totLen++;
                    break;
                case 'c':
                    *pStr++ = (char) va_arg(ap, int);
                    totLen++;
                    break;
                case 'u': {
                    char digit[12];  // we will only use the ten last digits, i.e. 0xffffffff = 4294967295 => ten digits
                    uint32_t u = 11;
                    uint32_t integer = (uint32_t) va_arg(ap, int);

                    do {
                        digit[u--] = (integer % 10) + '0';
                    } while (integer /= 10);
                
                    for (u++; u < 12; u++) {
                        *pStr++ = digit[u];
                        totLen++;
                    }
                    
                    break;
                } // case
				case 'p': {
				    uint32_t integer = (uint32_t) va_arg(ap, int), digit;
				    int32_t shift = 28;
				    
				    *pStr++ = '0';
				    *pStr++ = 'x';
				    totLen += 2;
				    
				    while (shift >= 0) {
				        digit = (integer >> shift) & 0xf;
				        if (digit < 10) *pStr++ = digit + '0';
				        else *pStr++ = digit + 'a' - 10;
				        totLen++;
				        shift -= 4;
				    } // while

				    break;
				} // case
				case 'x': {             // this generate wrong result when the input is zero...
				    uint32_t integer = (uint32_t) va_arg(ap, int), digit;
				    int32_t shift = 28;
				    uint32_t print_it = 0;
				    
				    *pStr++ = '0';
				    *pStr++ = 'x';
				    totLen += 2;
				    
				    while (shift >= 0) {
				        if ( (digit = (integer >> shift) & 0xf) ) print_it = 1;
				        //digit = (integer >> shift) & 0xf;
				        if (print_it) {
				            if (digit < 10) *pStr++ = digit + '0';
				            else *pStr++ = digit + 'a' - 10;
				            totLen++;
				        }
				        shift -= 4;
				    } // while
				    
				    // if zero
				    if ( *(pStr - 1) == 'x' ) {
				        *pStr++ = '0';
				        totLen++;
				    }

				    break;
				} // case
				case 's': {
				    const char *p = (const char *) va_arg(ap, int);
				    
				    while (*p) {
				        *pStr++ = *p++;
				        totLen++;
				    } // while
				    break;
				} // case
				default:
					va_end(ap);
				    LOG_WARNING_RETURN_NEGATIVE(0);
					break;
            } // switch
        } // else

        ASSERT( !((totLen) >= len) );

    } // while
    
    *pStr++ = '\0';
    puts( (const char *) str, len );
	va_end(ap);
	return totLen;
}


void print_hex32(const uint32_t u, uint32_t newline)
{
    uint32_t digit;
    int32_t shift = 28;


    puts("0x",10);
    
    while (shift >= 0) {
        digit = (u >> shift) & 0xf;
		if (digit < 10) putc(digit + '0');
		else putc(digit + 'a' - 10);
        shift -= 4;
    } // while
    
    if (newline) puts("\r\n", 10);
    
    return;
}