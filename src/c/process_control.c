/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-07-13
 *      Updated:    2016-06-15
 *
 *      Project:    bOS
 *      File name:  process_control.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       Updated
 *      ver 8       Updated
 *      ver 9       Added methods for a processes to hand over to an other 
 *                  process. Added methods for processes to enable and disable 
 *                  interrupts
 *      ver 13      Secured a critical region
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stddef.h>
#include "process_control.h"
#include "assert.h"
#include "io.h"
#include "sys_def.h"
#include "message.h"        // test only

extern int32_t _printf(const uint32_t len, const char *format, ...);

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

/* Context switch counter that increase by one every context switch. 
 * If you run the system for more then a year, change to uint64_t instead...
 */
static uint32_t gCntxtCnt = 0;

/* The process currently running. The first process is the idle process that
 * initiate the system and when all are up it's not doing anything more then
 * being a process that alway's can run if no one else would like to do so.
 */
pcb_t *gpCurrPCB = pcbList;

void proc_ctrl_create_pcb(create_pcb_t *p)
{
    pcb_t *pPCB = pcbList + p->pid;
    char *pSource = NULL;
    char *pDest = NULL;

    // pid
    ASSERT(HIGH_PID >= p->pid);
    pPCB->pid = p->pid;

    // process name
    ASSERT((sizeof(p->procName) >> 3) <= MAXLEN_PROCNAME - 1);
    pSource = p->procName;
    pDest = pPCB->procName;
    while ('\0' != *pSource) {
        *pDest = *pSource;
        pSource++;
        pDest++;
    }
    *pDest = '\0';

    // function pointer to the start function
    ASSERT(NULL != p->pFnct);
    pPCB->lr_exc = ((uint32_t) p->pFnct) + 4;

    // prio
    pPCB->prio = p->prio;

    // stack
    pPCB->r13 = p->stackTop;
    pPCB->stackTop = p->stackTop;
    pPCB->stackSize = p->stackSize;

    // mode
    pPCB->mode = p->mode;

#ifdef ARM32
    // Saved Program Status Register - spsr (ARM)
    pPCB->spsr = SPSR_CREATE_PCB | p->mode;

    // Zero out all others
    pPCB->r0 = 0;
    pPCB->r1 = 0;
    pPCB->r2 = 0;
    pPCB->r3 = 0;
    pPCB->r4 = 0;
    pPCB->r5 = 0;
    pPCB->r6 = 0;
    pPCB->r7 = 0;
    pPCB->r8 = 0;
    pPCB->r9 = 0;
    pPCB->r10 = 0;
    pPCB->r11 = 0;
    pPCB->r12 = 0;
    pPCB->r14 = 0;
#endif /* ARM32 */

    pPCB->totRuns = 0;
    pPCB->totReady = 0;
    pPCB->totNotFullCycle = 0;
    pPCB->lstCntxtCnt = 0;

    // process state
    if (PID_IDLE != p->pid) pPCB->state = created;
    else pPCB->state = running;                 // the running (idle) process

    return;
}



void proc_ctrl_init_proc(void)
{
    pcb_t *pPCB = pcbList;
    pcb_t *pPCB_Last = pcbList + HIGH_PID;
    procPrio_t prevPrio = prio_0;
    uint32_t pid = 1;

    // the idle process is already up and running
    pPCB->state = running;

    // init and check prio on the other processes
    pPCB++;
    while (pPCB_Last >= pPCB) {
        pPCB->state = ready;
        
        // check that the PCB is sorted in prio order
        ASSERT(prevPrio <= pPCB->prio);
        prevPrio = pPCB->prio;
        
        // check that respective PCB has correct pid
        ASSERT(pid++ == pPCB->pid);
        
        pPCB++;
    }
    return;
}



char *proc_ctrl_get_curr_proc_name(void)
{
    return gpCurrPCB->procName;
}



uint32_t proc_ctrl_get_curr_pid(void)
{
    return gpCurrPCB->pid;
}



procPrio_t proc_ctrl_get_curr_prio(void)
{
    return gpCurrPCB->prio;
}



void proc_ctrl_context_switch(const procState_t returnState)
{
    ASSERT(running < returnState);      // only accept process state ready and different versions of blocked
    ASSERT(0x100 == SYSTEM_CALL_CONTEXT_SWITCH);
    gpCurrPCB->totNotFullCycle++;
    
    DISABLE_INTERRUPT;
#ifdef ARM32                // swi is backward compatible on ARMv7 as well but is called svc instead
    __asm__ __volatile__ ("mov    r0, %[returnState]    \n\t"
                          "swi    0x100                 \n\t"   // see ASSERT above
                          : /* no output */
                          : [returnState] "r" (returnState)
                          : "cc", "r0", "memory");
#endif /* ARM32 */
    ENABLE_INTERRUPT;

    return;
}



void proc_ctrl_change_state(const uint32_t pid, const procState_t newState)
{
    pcb_t *p = pcbList + pid;
    
    if (running == p->state && ready == newState) return;

    ASSERT(running < newState);                         // only accept ready and different versions of blocked
    ASSERT(HIGH_PID >= pid);                            // the process must exist
    ASSERT(0 != pid);                                   // don't change state on the idle process
    ASSERT(!(ready == p->state && newState != ready));  // don't change processes that are in ready state
    ASSERT(running != p->state);                        // don't change state on the running process,
                                                        // use proc_ctrl_context_switch(...) if it's the current proc.
    // critical section starts
    DISABLE_INTERRUPT;
    p->state = newState;
    ENABLE_INTERRUPT;
    // critical section ends

    return;
}


/* This function should used with care, the critical regions are not protected 
   and this must be handled elseware; if not use the function
   "proc_ctrl_change_state" instead. */
void _proc_ctrl_change_state(const uint32_t pid, const procState_t newState)
{
    pcb_t *p = pcbList + pid;
    
    if (running == p->state && ready == newState) return;

    ASSERT(running < newState);                         // only accept ready and different versions of blocked
    ASSERT(HIGH_PID >= pid);                            // the process must exist
    ASSERT(0 != pid);                                   // don't change state on the idle process
    ASSERT(!(ready == p->state && newState != ready));  // don't change processes that are in ready state
    ASSERT(running != p->state);                        // don't change state on the running process,
                                                        // use proc_ctrl_context_switch(...) if it's the current proc.
    // critical section starts
    p->state = newState;
    // critical section ends

    return;
}


/*
* Schedule a new process to run after a context switch. This function
* are used only when a timer interrupt occurs or when we have a swi interrupt
* via proc_ctrl_context_switch() called by a process that would like to hand
* over to an other process.
*
* Note: this process contain a critical region and must not run if the
* interrupt isn't disabled (i.e. only the two functions described above should
* run this function)
*/
void _proc_ctrl_schedule(const procState_t returnState)
{
    pcb_t *p = pcbList;                         // point at the first process, the idle process
    const pcb_t *pPCB_ListEnd = pcbList + HIGH_PID;
    
    ASSERT(created != returnState && running != returnState);

    // critical section starts - however this function always runs in
    // interrupt disabled mode so we are safe

    // set the current process, if not the idle process, in return state
    if (gpCurrPCB != pcbList) gpCurrPCB->state = returnState;
    else gpCurrPCB->state = ready;

    // update number of runs (used for tuning the system and can be removed when
    // this is done)
    gpCurrPCB->lstCntxtCnt = ++gCntxtCnt;
    if (ready == returnState) {
        gpCurrPCB->totRuns++;
        gpCurrPCB->totReady++;
    }
    else gpCurrPCB->totRuns++;

    // find the next process to run (this linear search is quite slow. However,
    // the alternative is to have different sorted lists to search in and this,
    // due to the low number of processes most likely used, will take just as
    // long time)
    gpCurrPCB = pcbList;    // point at the first process, the idle process
    while (p <= pPCB_ListEnd) {
        if (gpCurrPCB->prio > p->prio && ready == p->state)
            gpCurrPCB = p;
        if (gpCurrPCB->prio == p->prio && gpCurrPCB->lstCntxtCnt > p->lstCntxtCnt && ready == p->state)
            gpCurrPCB = p;

        // all processes below have lower prio, so break...
        if (gpCurrPCB->prio < p->prio) break;

        p++;
    }

    // change state on the new process
    gpCurrPCB->state = running;

    // critical sections ends

    return;
}



void proc_ctrl_print_pcb(uint32_t pid)
{
    pcb_t *p = pcbList + pid;

    // processor registers
    _printf(999, "\n PCB at %p:\
                  \n   r0 =             %p\
                  \n   r1 =             %p\
                  \n   r2 =             %p\
                  \n   r3 =             %p\
                  \n   r4 =             %p\
                  \n   r5 =             %p\
                  \n   r6 =             %p\
                  \n   r7 =             %p\
                  \n   r8 =             %p\
                  \n   r9 =             %p\
                  \n   r10 =            %p\
                  \n   r11 =            %p\
                  \n   r12 =            %p\
                  \n   r13 =            %p\
                  \n   r14 =            %p\
                  \n   spsr =           %p\
                  \n   lr_exc =         %p\n", 
                  p, p->r0, p->r1, p->r2, p->r3, p->r4, p->r5, p->r6, p->r7, p->r8, 
                  p->r9, p->r10, p->r11, p->r12, p->r13, p->r14, p->spsr, p->lr_exc);

    // others
    _printf(999, "\
                  \n   pid =             %p\
                  \n   prio =            %p\
                  \n   mode =            %p\
                  \n   state =           %p\
                  \n   stackTop =        %p\
                  \n   stackSize =       %p\
                  \n   procName =        %s\
                  \n   totRuns =         %p\
                  \n   totReady =        %p\
                  \n   totNotFullCycle = %p\
                  \n   lstCntxtCnt =     %p\n",
                  p->pid, p->prio, p->mode, p->state, p->stackTop, p->stackSize, 
                  p->procName, p->totRuns, p->totReady, p->totNotFullCycle, p->lstCntxtCnt);

    return;
}



void proc_ctrl_print_curr_pcb(void)
{
    proc_ctrl_print_pcb(proc_ctrl_get_curr_pid());
    return;
}


// ******** Test only ***********


void _proc_ctrl_print_status(void)
{
    uint32_t pid;

    _printf(100, "%u [%u (%u)", gpCurrPCB->pid, pcbList[0].pid, pcbList[0].state);
    for (pid = 1; pid <= HIGH_PID; pid++) {
        _printf(100, ", %u (%u)", pcbList[pid].pid, pcbList[pid].state);
    } 
    _printf(100, "]\n");

    return;
}

void show_process_statistic(void)
{
    static uint32_t lstCntxtCnt = 0;
/*
    if (lstCntxtCnt < pcbList[proc_ctrl_get_curr_pid()].lstCntxtCnt) {
        DISABLE_INTERRUPT;
        lstCntxtCnt = pcbList[proc_ctrl_get_curr_pid()].lstCntxtCnt;
        printf(100, "\r curr proc: %u | ", proc_ctrl_get_curr_pid());
        for (uint32_t pid = 0; pid < NUMB_PROC; pid++) {
            printf(100, "%u, %u, %u, %u | ", 
            pcbList[pid].totRuns, pcbList[pid].totReady, pcbList[pid].totNotFullCycle, pcbList[pid].lstCntxtCnt);
        }
        puts("\n", 10);

        ENABLE_INTERRUPT;
    }
*/

    lstCntxtCnt++;
    return;
}

uint32_t run = 0;

void procA(void)
{
    uint32_t pid = proc_ctrl_get_curr_pid();
    message_t *msg = NULL;
    

    printf(100, "PID %u: process %s has started...\n", proc_ctrl_get_curr_pid(), proc_ctrl_get_curr_proc_name());
  
    // send some messages to myself and to my friends
    msg = msg_create(3, msg_ping, NULL);    // procA
    msg_send(msg);
    msg = msg_create(4, msg_ping, NULL);    // procB
    msg_send(msg);
    msg = msg_create(5, msg_ping, NULL);    // procC
    msg_send(msg);
    msg = msg_create(6, msg_ping, NULL);    // procD
    msg_send(msg);

    while (1) {
        // fall into sleep if no messages are to be found in the que..
        msg_sleep();
        msg = msg_receive();
        if (NULL != msg && msg_ping == msg_get_id(msg)) {
            printf(100, "Process with PID %u received a PING from process %u...\n", pid, msg_get_sender(msg));
            msg_delete(msg);
        }
    }

    return;
}


void procB(void)
{
    uint32_t pid = proc_ctrl_get_curr_pid();
    message_t *msg = NULL;


    printf(100, "PID %u: process %s has started...\n", proc_ctrl_get_curr_pid(), proc_ctrl_get_curr_proc_name());

    // send some messages to myself and to my friends
    msg = msg_create(3, msg_ping, NULL);    // procA
    msg_send(msg);
    msg = msg_create(4, msg_ping, NULL);    // procB
    msg_send(msg);
    msg = msg_create(5, msg_ping, NULL);    // procC
    msg_send(msg);
    msg = msg_create(6, msg_ping, NULL);    // procD
    msg_send(msg);

    while (1) {
        // fall into sleep if no messages are to be found in the que..
        msg_sleep();
        msg = msg_receive();
        if (NULL != msg && msg_ping == msg_get_id(msg)) {
            printf(100, "Process with PID %u received a PING from process %u...\n", pid, msg_get_sender(msg));
            msg_delete(msg);
        }
    }

    return;
}


void procC(void)
{
    uint32_t pid = proc_ctrl_get_curr_pid();
    message_t *msg = NULL;


    printf(100, "PID %u: process %s has started...\n", proc_ctrl_get_curr_pid(), proc_ctrl_get_curr_proc_name());

    // send some messages to myself and to my friends
    msg = msg_create(3, msg_ping, NULL);    // procA
    msg_send(msg);
    msg = msg_create(4, msg_ping, NULL);    // procB
    msg_send(msg);
    msg = msg_create(5, msg_ping, NULL);    // procC
    msg_send(msg);
    msg = msg_create(6, msg_ping, NULL);    // procD
    msg_send(msg);

    while (1) {
        // fall into sleep if no messages are to be found in the que..
        msg_sleep();
        msg = msg_receive();
        if (NULL != msg && msg_ping == msg_get_id(msg)) {
            printf(100, "Process with PID %u received a PING from process %u...\n", pid, msg_get_sender(msg));
            msg_delete(msg);
        }
    }

    return;
}


void procD(void)
{
    uint32_t pid = proc_ctrl_get_curr_pid();
    message_t *msg = NULL;
    
    
    printf(100, "PID %u: process %s has started...\n", proc_ctrl_get_curr_pid(), proc_ctrl_get_curr_proc_name());

    // send some messages to myself and to my friends
    msg = msg_create(3, msg_ping, NULL);    // procA
    msg_send(msg);
    msg = msg_create(4, msg_ping, NULL);    // procB
    msg_send(msg);
    msg = msg_create(5, msg_ping, NULL);    // procC
    msg_send(msg);
    msg = msg_create(6, msg_ping, NULL);    // procD
    msg_send(msg);

    while (1) {
        // fall into sleep if no messages are to be found in the que..
        msg_sleep();
        msg = msg_receive();
        if (NULL != msg && msg_ping == msg_get_id(msg)) {
            printf(100, "Process with PID %u received a PING from process %u...\n", pid, msg_get_sender(msg));
            msg_delete(msg);
        }
    } 

    return;
}



