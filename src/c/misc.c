/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-09-01
 *      Updated:    2016-05-18
 *
 *      Project:    bOS
 *      File name:  misc.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 10      Created
 *      ver 12      Added memcmp(...)
 *
 *
 *      Reference: 
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "assert.h"
#include "io.h"


static uint32_t atoi_dec(char *str);
static uint32_t atoi_hex(char *str);


int32_t strncmp(const char *str1, const char *str2, uint32_t len)
{
    ASSERT(NULL != str1);
    ASSERT(NULL != str2);
    ASSERT(len > 0);

    while (0 < len) {

        if (*str1 > *str2) {
            return 1;
        }
        else if (*str1 < *str2) {
            return -1;
        }

        if ('\0' == *str1 && '\0' == *str2) {
            break;
        }

        str1++;
        str2++;
        len--;
    }

    return 0;
}


int32_t memcmp(const void *ptr1, const void *ptr2, uint32_t len)
{
    const char *str1 = (const char *)ptr1;
    const char *str2 = (const char *)ptr2;

    ASSERT(NULL != str1);
    ASSERT(NULL != str2);
    ASSERT(len > 0);

    while (0 < len) {

        if (*str1 > *str2) {
            return 1;
        }
        else if (*str1 < *str2) {
            return -1;
        }

        str1++;
        str2++;
        len--;
    }

    return 0;
}


int32_t atoi(char *str)
{
    int32_t nbr = 0;
    int32_t sign = 1;

    if (NULL != str) {
        if ('-' == *str) {
            sign = -1;
            str++;
        }
        
        if ('0' == *str && '\0' == *(str + 1)) {        // Zero...
            nbr = 0;
        }
        else if ('0' == *str && 'x' == *(str + 1)) {    // Hexadecimal representation (0x...)
            nbr = atoi_hex(str + 2);
        }
        else if ('0' == *str && 'X' == *(str + 1)) {    // Hexadecimal representation (0X...)
            nbr = atoi_hex(str + 2);
        }
        else if ('1' <= *str && *str <= '9') {          // Decimal representation
            nbr = atoi_dec(str);
        }
        else {
            printf(100, "warning@atoi: input is not an integer (hex or dec) representation, %s\n", str);
            printf(100, "   return zero (0)...\n");
            //ASSERT(0);
        }
    }
    else {
        printf(100, "error@atoi: input is a NULL terminated string\n");
        printf(100, "   return zero (0)...\n");
        ASSERT(0);
    }

    return nbr*sign;
}


static uint32_t atoi_dec(char *str)
{
    uint64_t nbr = 0;
    uint32_t n = 0;
    
    while (1) {
        if ('0' <= *str && *str <= '9') {
            nbr = nbr*10 + (*str - '0');
            str++;
            n++;
        }
        else if ('\0' == *str) {
            break;
        }
        else {
            //ASSERT(0);
            printf(100, "warning@atoi_dec: input is not an integer representation, %s\n", (str - n));
            printf(100, "   return zero (0)...\n");
            nbr = 0;
            break;
        }
        
        if (UINT32_MAX < nbr) {
            //ASSERT(0);
            printf(100, "warning@atoi_dec: input is larger then UINT32_MAX, %s\n", (str - n));
            printf(100, "   return zero (0)...\n");
            nbr = 0;
            break;
        }
    }

    return (uint32_t) nbr;
}


static uint32_t atoi_hex(char *str)
{
    uint64_t nbr = 0;
    uint32_t n = 0;
    
    while (1) {
        if ('0' <= *str && *str <= '9') {
            nbr = nbr*0x10 + (*str - '0');
            str++;
            n++;
        }
        else if ('a' <= *str && *str <= 'f') {
            nbr = nbr*0x10 + (*str - 'a' + 10);
            str++;
            n++;
        }
        else if ('A' <= *str && *str <= 'F') {
            nbr = nbr*0x10 + (*str - 'A' + 10);
            str++;
            n++;
        }
        else if ('\0' == *str) {
            break;
        }
        else {
            //ASSERT(0);
            printf(100, "warning@atoi_hex: input is not an integer representation, %s\n", (str - n - 2));
            printf(100, "   return zero (0)...\n");
            nbr = 0;
            break;
        }
        
        if (UINT32_MAX < nbr) {
            //ASSERT(0);
            printf(100, "warning@atoi_hex: input is larger then UINT32_MAX, %s\n", (str - n - 2));
            printf(100, "   return zero (0)...\n");
            nbr = 0;
            break;
        }
    }
    
    return (uint32_t) nbr;
}