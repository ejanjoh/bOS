/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-05-30
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  init.c
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

#include <stdint.h>
#include "hardware_system.h"
#include "io.h"
#include "process_control.h"


extern uint32_t _start;                 // from linker script
extern uint32_t __stack_sys_top;        // from linker script
extern void main(void);                 // found in main.c
extern void procA(void);                // found in process_control.c
extern void procB(void);                // found in process_control.c
extern void procC(void);                // found in process_control.c
extern void procD(void);                // found in process_control.c


// Respective process stack top and size, the base for this define are to be found in
// the linker script kernel.ld
#define STACK_SIZE          0x200           // stack_block_size    = 0x200;, see linker script
#define STACK_PTR(pid)      ((uint32_t) &__stack_sys_top - (pid)*STACK_SIZE)


// Input to create the processes in the system
static create_pcb_t init_pcb[NUMB_PROC] = {
    {0, "idle", main, prio_idle_proc, STACK_PTR(0), STACK_SIZE, modeSys},
    {1, "procA", procA, prio_5, STACK_PTR(1), STACK_SIZE, modeSys},
    {2, "procB", procB, prio_5, STACK_PTR(2), STACK_SIZE, modeSys},
    {3, "procC", procC, prio_5, STACK_PTR(3), STACK_SIZE, modeSys},
    {HIGH_PID, "procD", procD, prio_5, STACK_PTR(HIGH_PID), STACK_SIZE, modeSys}};

extern void init_context_switch_timer2(void);

void init(void)
{
    printf(100, "System started at: %p\r\n", &_start);

    // Create the PCB
    for (uint32_t pcb = 0; pcb < NUMB_PROC; pcb++) proc_ctrl_create_pcb(init_pcb + pcb);
    
    // test
    //proc_ctrl_print_pcb(4);

    // Set up the timer needed for the interrups
    init_context_switch_timer2();

    // Set up the interrupt controller
    // - Assign priority and select IRQ
    SET32(INTC_BASE + INTC_ILR68_OFFSET, (~INTC_FIQ_IRQ_MASK & 0x00) | INTC_PRIO_0_PAT);
    // - Set up the interrupt mask
    SET32(INTC_BASE + INTC_MIR_CLEAR2_OFFSET, INTC_MIR2_MASK);

    return;
}


