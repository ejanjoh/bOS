/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-09-01
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  misc.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 10      created
 *
 *
 *      Reference: 
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "assert.h"

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