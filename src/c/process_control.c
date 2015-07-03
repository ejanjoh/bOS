/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-07-13
 *      Updated:    2015-06-03
 *
 *      Project:    bOS
 *      File name:  process_control.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       Updated
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "process_control.h"
#include "io.h"



// The total number of processes used in the system
#define NUMB_PROC 1


/* Process Control Block list.
 *
 * The list is used to store process information between context switches. It's
 * also used as list for processes in different state, e.g. to find the next
 * process to run after a context switch or storing processes blocked and
 * waiting for something to happened. It's a complete list of all processes
 * used in the system and it must be sorted in prio order, apart from the first
 * process - the idle process (pid = 0).
 */
static pcb_t pcbList[NUMB_PROC];

/* The process currently running. The first process is the idle process that
 * initiate the system and when all are up it's not doing anything more then
 * being a process that alway's can run if no one else would like to do so.
 */
pcb_t *gpCurrPCB = pcbList;



void print_current_pcb(void)
{
    printf(999, "\r\n gpCurrPCB:\r\n\
                 r0 =      %p\r\n\
                 r1 =      %p\r\n\
                 r2 =      %p\r\n\
                 r3 =      %p\r\n\
                 r4 =      %p\r\n\
                 r5 =      %p\r\n\
                 r6 =      %p\r\n\
                 r7 =      %p\r\n\
                 r8 =      %p\r\n\
                 r9 =      %p\r\n\
                 r10 =     %p\r\n\
                 r11 =     %p\r\n\
                 r12 =     %p\r\n\
                 r13 =     %p\r\n\
                 r14 =     %p\r\n\
                 spsr =    %p\r\n\
                 lr_exc =  %p\r\n\r\n", 
                 gpCurrPCB->r0, gpCurrPCB->r1, gpCurrPCB->r2, gpCurrPCB->r3,
                 gpCurrPCB->r4, gpCurrPCB->r5, gpCurrPCB->r6, gpCurrPCB->r7,
                 gpCurrPCB->r8, gpCurrPCB->r9, gpCurrPCB->r10, gpCurrPCB->r11, 
                 gpCurrPCB->r12, gpCurrPCB->r13, gpCurrPCB->r14, gpCurrPCB->spsr, gpCurrPCB->lr_exc);

    return;
}