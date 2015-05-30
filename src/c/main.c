/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-28
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  main.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 6       Updated
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "io.h"
#include "log.h"
#include "assert.h"



void test(uint32_t bool)
{
    LOG_WARNING_RETURN(bool);
}


void main(void)
{
    uint32_t u;


    test(0);
    
    printf(100, "Add some chars %c%c%c\r\n", 'A', 'B', 'C');
    
    for (u = 0; u <= 16; u++) 
        printf(500, "Unsigned integer: %u, and as hexadecimal : %x (%p)\r\n", u, u, u);
        
    printf(100, "this is an other test... %x\r\n", 66081);      // should be 0x10221

    ASSERT(0);

    // Never end...
    for (;;) ;
    ASSERT(0);
}