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
 *      ver 8       Updated
 *      ver 9       Added methods for a processes to hand over to an other 
 *                  process. Added methods for processes to enable and disable 
 *                  interrupts
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
#ifndef PROCESS_CONTROL_H_
#define PROCESS_CONTROL_H_

#include <stdint.h>
#include "hardware_system.h"

#ifdef ARM_SYS32_MODE_ONLY
// Disable interrupts (irq)

extern int32_t gDisableInterruptCount;

#define DISABLE_INTERRUPT __asm__ __volatile__ ("mrs     r3, cpsr\n\t"  \
                                                "orr     r3, r3, #(0x80)\n\t" \
                                                "msr     cpsr_c, r3\n\t" ::: "r3", "cc", "memory")
// Enable interrupts (irq)
#define ENABLE_INTERRUPT __asm__ __volatile__ ("mrs     r3, cpsr\n\t"  \
                                               "bic     r3, r3, #(0x80)\n\t" \
                                               "msr     cpsr_c, r3\n\t" ::: "r3", "cc", "memory")
#else
#define DISABLE_INTERRUPT ((void) 0)
#define ENABLE_INTERRUPT ((void) 0)
#endif /* ARM_SYS32_MODE_ONLY */

// The max length of a process name (MAXLEN_PROCNAME-1)
#define MAXLEN_PROCNAME 32

// The total number of processes used in the system
#define NUMB_PROC 5

// The highest pid used in the system
#define HIGH_PID  NUMB_PROC - 1

/* Process state - type definition
 *
 * Description: procState_t defines the different states the processes in the
 *              system can take. The base states are: created, running, ready
 *              and blocked (the state transitions are described below).
 *              Additional sub-states to blocked can be defined and should be
 *              placed before the state blocked. The sub-states should behave
 *              as the state blocked during state transitions.
 *
 * Process state transitions (the state machine):
 * created --> ready
 * running --> ready or blocked
 * ready   --> running
 * blocked --> ready
 */
typedef enum {created = 0, running, ready, blocked_semaphore, blocked} procState_t;

/* Process prio - type definition
 *
 * Description: procPrio_t defines the different priorities that the processes
 *              in the system can take. The lowest value has the highest
 *              process priority, i.e. lower number will run first. The
 *              priority prio_idle_proc is reserved for the idle process and
 *              should not be used by other processes.
 */
typedef enum {  prio_0 = 0, prio_1, prio_2, prio_3, prio_4, prio_5, prio_6,
                prio_7, prio_8, prio_9, prio_10, prio_11, prio_12, prio_13,
                prio_14, prio_15, prio_16, prio_17, prio_18, prio_19, prio_20,
                prio_21, prio_22, prio_23, prio_24, prio_25, prio_26, prio_27,
                prio_28, prio_29, prio_idle_proc
             } procPrio_t;

// Process modes - type definition
#ifdef ARM32
typedef enum {modeUsr = USR32_MODE, modeSys = SYS32_MODE} procMode_t;
#endif /* ARM32 */

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
uint32_t r13;                               // stack pointer (sp)
uint32_t r14;                               // link register (ln)
uint32_t spsr;                              // saved program status register
uint32_t lr_exc;
#endif /* ARM32 */

// ********** User, Context and Schedule dependencies *********************
uint32_t pid;                               // process id
procPrio_t prio;                            // process prio
procMode_t mode;                            // process mode (usr or sys)
procState_t state;                          // the state of the process
uint32_t stackTop;                          // the stack top
uint32_t stackSize;                         // the size of the process stack
char procName[MAXLEN_PROCNAME];             // process name

// ********** Debug and Tuning ********************************************
uint32_t totRuns;                           // the number of times the process has been running
uint32_t totReady;                          // the number of times the process has switched out and been ready to run.
uint32_t totNotFullCycle;                   // the number of times the process has not run a full cycle.
uint32_t lstCntxtCnt;                       // the last count of gCntxtCnt when this process was running
} pcb_t;



typedef struct create_pcb {
    uint32_t pid; 
    char procName[MAXLEN_PROCNAME];
    void (*pFnct)(void);
    const procPrio_t prio;
    uint32_t stackTop;
    uint32_t stackSize;
    procMode_t mode;
} create_pcb_t;



/* void proc_ctrl_create_pcb(create_pcb_t *p)
 *
 * p:           Pointer to a struct containing parameters needed for the setup 
 *              and initialization of the PCB.
 *
 * out:         None
 *
 * description: This function, during the start-up of the system, creates a PCB
 *              in the PCB-list for the function defined by the in-parameter.
 *
 * depend:      None
 *
 * note:        The function should be performed before we start any processes
 *              at all.
 */
void proc_ctrl_create_pcb(create_pcb_t *p);



/* void proc_ctrl_init_proc(void)
 *
 * in:          None
 *
 * out:         None
 *
 * description: proc_ctrl_init_proc performs some checks on the content within the
 *              PCB-list and on error it will assert the system. It also prepare
 *              the processes within the PCB-list to run. After this is done,
 *              the systems all processes are ready to run.
 *
 * depend:      ProcCtrlCreatePCB(...)
 *
 * note:        The function should be performed before we start any processes
 *              at all.
 */
void proc_ctrl_init_proc(void);



/* char *proc_ctrl_get_curr_proc_name(void)
 *
 * in:          None
 *
 * out:         The name of the current running process
 *
 * description: See description on out.
 *
 * depend:      None
 *
 * note:        None
 */
char *proc_ctrl_get_curr_proc_name(void);



/* uint32_t proc_ctrl_get_curr_pid(void)
 *
 * in:          None
 *
 * out:         The process id on the process currently running.
 *
 * description: See description on out.
 *
 * depend:      None
 *
 * note:        None
 */
uint32_t proc_ctrl_get_curr_pid(void);



/* procPrio_t proc_ctrl_get_curr_prio(void)
 *
 * in:          None
 *
 * out:         The process priority on the process currently running.
 *
 * description: See description on out.
 *
 * depend:      None
 *
 * note:        None
 */
procPrio_t proc_ctrl_get_curr_prio(void);



/* void proc_ctrl_context_switch(const procState_t returnState)
 *
 * returnState: The state of the process switched out after a context switch
 *              initiated by this currently running process.
 *
 * out:         None
 *
 * description: ProcCtrlContextSwitch perform a context switch initiated by the
 *              currently running process. This context switch is not the same
 *              as the one been performed by the one occurring after the system
 *              timer interrupt. It should be used by a process that would like
 *              to hand over another process, e.g. if the current process is
 *              waiting for something to happened. The new state for the
 *              current process is specified by the returnState.
 *
 * depend:      SWI and ProcCtrlSchedule(...)
 *
 * note:        This function will while handling critical regions in the PCB
 *              disable interrupts. If any errors occur during the execution
 *              of this function, the system will hang and an error note will
 *              be printed out.
 */
void proc_ctrl_context_switch(const procState_t returnState);



/* void proc_ctrl_change_state(const uint32_t pid, const procState_t newState)
 *
 * returnState: The state of the process scheduled out after a context switch.
 *
 * pid:         The process id of the process to change state on.
 * newState:    The new state on the process
 *
 * out:         None
 *
 * description: Change process state of a process in state, or some sub-state,
 *              to blocked. It's possible to change to another blocked state or
 *              to the state ready. It is not possible to change state on
 *              processes in state running or ready, nor it isn't possible to
 *              change state of the idle process.
 *
 * depend:      None
 *
 * note:        This function contain a critical region and due to the fact
 *              that we do a change in the PCB the interrupts are disabled
 *              during the change within PCB. This function will hang on error.
 */
void proc_ctrl_change_state(const uint32_t pid, const procState_t newState);



/* void proc_ctrl_print_pcb(uint32_t pid)
 *
 * pid:         The process id (pid)
 *
 * out:         None
 *
 * description: This function print out the PCB for a process with process id
 *              as described by the in-parameter
 *
 * depend:      None
 *
 * note:        The function should be used with some caution due to the risk
 *              that the content of the PCB could change if a context switch
 *              occur during the printout. This may be changed by adding a
 *              protection for the critical region - to be done.
 */
void proc_ctrl_print_pcb(uint32_t pid);


#endif /* PROCESS_CONTROL_H_ */