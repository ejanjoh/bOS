/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-17
 *      Updated:    2015-09-08
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
 *      ver 10      Updated for handling I/O stream buffers
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
#include "io.h"
#include "sys_def.h"
#include "process_control.h"
#include "semaphore.h"


#define IO_EOL_CHAR '\n'

void (* _putc)(const char ch) = NULL;
void (* _puts)(const char *str, const uint32_t len) = NULL;
char (* _getc)(void) = NULL;

extern void _uart_putc(const char ch);
extern void _uart_puts(const char *str, const uint32_t len);
extern char _uart_getc(void);

extern semaphore_t gMutexIoOut;

void setup_default_io_config(void)
{    
    _putc = _uart_putc;
    _puts = _uart_puts;
    _getc = _uart_getc;
    
    _puts("Default out configured...\n", 100);
    _puts("Default in configured...\n", 100);

    return;
}


void putc(const char c)
{
    semaphore_wait(&gMutexIoOut);
    _io_buffer_write_char(&io_out, c);
    _io_buffer_write_char(&io_out, '\0');
    _io_buffer_add_line(&io_out);
    
    // semaphore_wait will generate a context switch. Make sure that the cli_io_out_proc
    // is in ready state.
    proc_ctrl_change_state(PID_CLI_IO_OUT, ready);
    semaphore_signal(&gMutexIoOut);
    
    return;
}


void puts(const char *str, const uint32_t len)
{
    ASSERT(sizeof(str) < len);
    semaphore_wait(&gMutexIoOut);
    _io_buffer_write_string(&io_out, str);

    // semaphore_wait will generate a context switch. Make sure that the cli_io_out_proc
    // is in ready state.
    proc_ctrl_change_state(PID_CLI_IO_OUT, ready);
    semaphore_signal(&gMutexIoOut);

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


int32_t _printf(const uint32_t len, const char *format, ...)
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
    _puts( (const char *) str, len );
	va_end(ap);
	return totLen;
}


void _print_hex32(const uint32_t u, uint32_t newline)
{
    uint32_t digit;
    int32_t shift = 28;


    _puts("0x",10);
    
    while (shift >= 0) {
        digit = (u >> shift) & 0xf;
		if (digit < 10) _putc(digit + '0');
		else _putc(digit + 'a' - 10);
        shift -= 4;
    } // while
    
    if (newline) _puts("\n", 10);
    
    return;
}


// Init a I/O buffer
void _io_buffer_init(io_buffer_t *io_buffer, char *buffer, const uint32_t size)
{
    fifo_init(&io_buffer->fifo, buffer, size);
    io_buffer->lines = 0;
    return;
}


// Get the number of lines added to the buffer from the buffer (not read yet)
uint32_t _io_buffer_get_lines(io_buffer_t *io_buffer)
{
    return io_buffer->lines;
}


// Write a char to a I/O buffer
void _io_buffer_write_char(io_buffer_t *io_buffer, const char c)
{
    fifo_write(&io_buffer->fifo, c);
    if (IO_EOL_CHAR == c) {
        fifo_write(&io_buffer->fifo, '\0');
        io_buffer->lines++;
    }
    return;
}


// Write a string to a I/O buffer
void _io_buffer_write_string(io_buffer_t *io_buffer, const char *s)
{
    char *p = (char *) s;
    
    ASSERT('\0' != *p);
    
    do {
        fifo_write(&io_buffer->fifo, *p);
        if (IO_EOL_CHAR == *p) {
            fifo_write(&io_buffer->fifo, '\0');
            io_buffer->lines++;
        }
    } while ('\0' != *(++p));

    return;
}


// Read a string from a I/O buffer
void _io_buffer_read_string(io_buffer_t *io_buffer, const char *s)
{
    char *p = (char *) s;
    ASSERT(io_buffer->lines > 0);
    
    do {
        *p = fifo_read(&io_buffer->fifo);
    } while ('\0' != *p++);

    io_buffer->lines--;
    return;
}


// Increase the counted number of lines in the buffer by one (to be used with care)
void _io_buffer_add_line(io_buffer_t *io_buffer)
{
    io_buffer->lines++;
    return;
}







