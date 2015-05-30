/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-17
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  io.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Created
 *      ver 5       Changed some function names to more common ones, even if
 *                  the interface differs somewhat.
 *      ver 6       Corrected the definition of putc and puts.
 *                  Added formated output by printf(...)
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#ifndef IO_H_
#define IO_H_

#include <stdarg.h>
#include <stdint.h>

/* void putc(const char ch)
 *
 * in:          ch:         character to be put transmitted to the out stream
 *
 * out:         none
 *
 * description: Put a character on the out stream. (currently the out
 *              stream is not implemented so it is mapped to the predefined 
 *              out (uart))
 *
 * depend:      None
 *
 * note:        None
 */
extern void (* putc)(const char ch);


/* void puts(const char *str, const uint32_t len)
 *
 * in:          str:        null terminated string of characters
 *              len:        max length of the string above
 *
 * out:         none
 *
 * description: Put a null terminated string on the predefined out stream for
 *              transmit. (currently the out stream is not implemented so it
 *              is mapped to the predefined out (uart))
 *
 * depend:      None
 *
 * note:        None
 */
extern void (* puts)(const char *str, const uint32_t len);


/* int32_t printf(const uint32_t len, const char *format, ...)
 *
 * in:          len:        max length of the string
 *              format:     string that could include format specifiers
 *
 *              format specifiers:
 *                  %u  unsigned decimal integer
 *                  %x  unsigned hexadecimal integer (lower case)
 *                  %p  pointer address
 *                  %c  character
 *                  %s  string of characters
 *                  %%  %
 *
 * out:         error on negative else the number ouf chars printed
 *
 * description: Writes the string pointed by format to the standard output.
 *              If format includes format specifiers (subsequences beginning 
 *              with %), the additional arguments following format are formatted 
 *              and inserted in the resulting string replacing their respective
 *              specifiers.
 *
 * depend:      None
 *
 * note:        None
 */
int32_t printf(const uint32_t len, const char *format, ...);

#endif /* IO_H_ */