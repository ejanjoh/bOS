/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-07-13
 *      Updated:    2015-06-03
 *
 *      Project:    bOS
 *      File name:  process_control.h
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
 
#ifndef PROCESS_CONTROL_H_
#define PROCESS_CONTROL_H_

#include <stdint.h>
#include "hardware_system.h"


// The max length of a process name (MAXLEN_PROCNAME-1)
#define MAXLEN_PROCNAME         32



// Process Control Block - type definition
typedef struct pcb {
// ********** Processor Register ******************************************
#ifdef ARM32
uint32_t r0;
uint32_t r1;
uint32_t r2;
uint32_t r3;
uint32_t r4;
uint32_t r5;
uint32_t r6;
uint32_t r7;
uint32_t r8;
uint32_t r9;
uint32_t r10;
uint32_t r11;
uint32_t r12;
uint32_t r13;                                   // stack pointer (sp)
uint32_t r14;                                   // link register (ln)
uint32_t spsr;                                  // saved program status register
uint32_t lr_exc;
#endif /* ARM32 */
/*
// ********** Context and Schedule dependencies ***************************
uint32_t pid;                                   // process id
procPrio_t prio;                                // process prio
procMode_t mode;                                // process mode (usr or system)
procState_t state;                              // the state of the process

// ********** User, Debug and Tuning **************************************
uint32_t stackTop;                              // the stack top
uint32_t stackSize;                             // the size of the process stack
uint64_t runTime;                               // time spent by this process
uint64_t lastTime;                              // process inactive since
uint64_t tickUsed;                              // the number of times the process has been
                                                                // running
uint64_t tickFullCycle;                         // the number of times the process has been running a full context cycle
char procName[MAXLEN_PROCNAME];                 // process name */
} pcb_t;





#endif /* PROCESS_CONTROL_H_ */