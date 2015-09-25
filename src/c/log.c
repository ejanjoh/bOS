/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-09-08
 *
 *      Project:    bOS
 *      File name:  log.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 6       Updated
 *      ver 10      Added function print_stack
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "process_control.h"

extern int32_t _printf(const uint32_t len, const char *format, ...);
extern void proc_ctrl_print_curr_pcb(void);
extern uint32_t *_get_stack_ptr(void);
extern pcb_t *gpCurrPCB;

void print_stack(void);

void log_warning(const char *file, const uint32_t line)
{
    _printf(200, "notification of WARNING in file %s, at line %u\n", file, line);
    // add dump of stack etc if needed...
    return;
}


void log_assert(const char *file, const uint32_t line)
{
    DISABLE_INTERRUPT;
    _printf(200, "ASSERT in file %s at line %u\n%s", file, line, " - the system will hang...\n\n");

    print_stack();
    proc_ctrl_print_curr_pcb();
    
    return;
}


void print_stack(void)
{
    uint32_t *stackBottom = _get_stack_ptr();
    uint32_t *p = (uint32_t *) gpCurrPCB->stackTop;
    uint32_t i;


    _printf(200, "The current stack:\n");
    
    i = 0;
    while (p >= stackBottom) {
        _printf(200, "%p (%p)   ", p, *p);
        i++;
        p--;
        if (4 == i) {
            _printf(100, "\n");
            i = 0;
        }
    }
    _printf(100, "\n");

    // print a bit below the current stack pointer...
    stackBottom -= 32;
    i = 0;
    while (p >= stackBottom) {
        _printf(200, "%p (%p)   ", p, *p);
        i++;
        p--;
        if (4 == i) {
            _printf(100, "\n");
            i = 0;
        }
    }
    _printf(100, "\n");

    return;
}


