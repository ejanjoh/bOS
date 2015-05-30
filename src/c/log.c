/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-05-28
 *
 *      Project:    bOS
 *      File name:  log.c
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


void log_warning(const char *file, const uint32_t line)
{

    printf(200, "notification of WARNING in file %s, at line %u\r\n", file, line);
    // add dump of stack etc if needed...
    return;
}


void log_assert(const char *file, const uint32_t line)
{
    printf(200, "ASSERT in file %s at line %u\r\n%s", file, line, " - the system will hang...\r\n");
    // add dump of stack etc if needed...
    return;
}





