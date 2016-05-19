/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-09-01
 *      Updated:    2016-05-19
 *
 *      Project:    bOS
 *      File name:  string.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 10      created
 *      ver 12      Added memcmp(...)
 *
 *
 *      Reference: 
 *
 ******************************************************************************/

#include <stdint.h>


#ifndef STRING_H_
#define STRING_H_

/* int32_t strncmp(const char *str1, const char *str2, uint32_t len)
 *
 * in:          str1:   String to be compared (null terminated)
 *              str2:   String to be compared (null terminated)
 *              len:    Max length to be compared
 *
 * out:         Zero if the strings are identical. Negative if the first 
 *              character that does not match has a lower value in pStr1 than 
 *              in pStr2. Positive if the first character that does not match 
 *              has a greater value in ptr1 than in ptr2.
 *
 * description: Compares up to len characters of the C string str1 to those of 
 *              the C string str2. This function starts comparing the first 
 *              character of each string. If they are equal to each other, it 
 *              continues with the following pairs until the characters differ, 
 *              until a terminating null-character is reached, or until num 
 *              characters match in both strings, whichever happens first.
 *
 * depend:      None
 *
 * note:        None
 */
int32_t strncmp(const char *str1, const char *str2, uint32_t len);


/* int32_t memcmp(const void *ptr1, const void *ptr2, uint32_t len)
 *
 * in:          ptr1:   pointer to memory to be compared
 *              str2:   pointer to memory to be compared 
 *              len:    length to be compared in bytes
 *
 * out:         if return value < 0 then it indicates ptr1 is less than ptr2.
 *              if return value > 0 then it indicates ptr2 is less than ptr1.
 *              if return value = 0 then it indicates ptr1 is equal to ptr2.
 *
 * description: Compares the first n bytes of memory area ptr1 and memory 
 *              area ptr2.
 *
 * depend:      None
 *
 * note:        None
 */
int32_t memcmp(const void *ptr1, const void *ptr2, uint32_t len);


#endif /* STRING_H_ */