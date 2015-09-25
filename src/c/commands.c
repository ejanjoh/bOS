/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-09-01
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  commands.c
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

#include "io.h"


// For test only
void cmd0(void)
{
    printf(100, "cmd0\n");
}


// For test only
void cmd1(const char *c)
{
    printf(100, "cmd1 %s\n", c);
}


// For test only
void cmd2(const char *c1, const char *c2)
{
    printf(100, "cmd2 %s %s\n", c1, c2);
}


// For test only
void cmd3(const char *c1, const char *c2, const char *c3)
{
    printf(100, "cmd3 %s %s %s\n", c1, c2, c3);
}


