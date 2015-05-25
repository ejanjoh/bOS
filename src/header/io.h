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
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#ifndef IO_H_
#define IO_H_


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
void putc(const char ch);


/* void uart_puts(const char *str, const uint32_t len)
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
void uart_puts(const char *str, const uint32_t len);


#endif /* IO_H_ */