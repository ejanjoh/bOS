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
 *      ver 9       Init of a mutex to protect UART_0
 *      ver 10      Enabled UART0 Rx interrupts
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "hardware_system.h"
#include "io.h"
#include "process_control.h"
#include "semaphore.h"
#include "sys_def.h"


extern uint32_t _start;                 // from linker script
extern uint32_t __stack_sys_top;        // from linker script

extern void main(void);                 // found in main.c
extern void procA(void);                // found in process_control.c
extern void procB(void);                // found in process_control.c
extern void procC(void);                // found in process_control.c
extern void procD(void);                // found in process_control.c
extern void cli_io_in_proc(void);       // found in cli.c
extern void cli_io_out_proc(void);      // found in cli.c

extern int32_t _printf(const uint32_t len, const char *format, ...);
extern void cli_io_init(void);

// Respective process stack top and size, the base for this define are to be found in
// the linker script kernel.ld
#define STACK_SIZE 5*0x400      // see linker script
#define STACK_PTR(pid) ((uint32_t) &__stack_sys_top - (pid)*STACK_SIZE)

// Input to create the processes in the system
static create_pcb_t init_pcb[NUMB_PROC] = {
    {PID_IDLE, "idle", main, prio_idle_proc, STACK_PTR(PID_IDLE), STACK_SIZE, modeSys},
    {PID_CLI_IO_IN, "cli_io_in", cli_io_in_proc, prio_4, STACK_PTR(PID_CLI_IO_IN), STACK_SIZE, modeSys},
    {PID_CLI_IO_OUT, "cli_io_out", cli_io_out_proc, prio_9, STACK_PTR(PID_CLI_IO_OUT), STACK_SIZE, modeSys},
    {PID_PROC_A, "procA", procA, prio_12, STACK_PTR(PID_PROC_A), STACK_SIZE, modeSys},
    {PID_PROC_B, "procB", procB, prio_12, STACK_PTR(PID_PROC_B), STACK_SIZE, modeSys},
    {PID_PROC_C, "procC", procC, prio_12, STACK_PTR(PID_PROC_C), STACK_SIZE, modeSys},
    {HIGH_PID, "procD", procD, prio_12, STACK_PTR(HIGH_PID), STACK_SIZE, modeSys}};


extern void init_context_switch_timer2(void);

void init(void)
{
    _printf(100, "System started at: %p\n", &_start);

    // Create the PCB
    for (uint32_t pcb = 0; pcb < NUMB_PROC; pcb++) proc_ctrl_create_pcb(init_pcb + pcb);
    
    // Init the "stdio" buffers used in the system
    cli_io_init();

    // Set up the timer needed for the interrups
    init_context_switch_timer2();

    // Set up the interrupt controller
    // - Assign priority and select IRQ (TIMER2 and UART0)
    SET32(INTC_BASE + INTC_ILR68_OFFSET, (~INTC_FIQ_IRQ_MASK & 0x00) | INTC_PRIO_0_PAT);
    SET32(INTC_BASE + INTC_ILR72_OFFSET, (~INTC_FIQ_IRQ_MASK & 0x00) | INTC_PRIO_0_PAT);
    // - Set up the interrupt mask (2nd interrupt bank, i.e. TIMER2 and UART0)
    SET32(INTC_BASE + INTC_MIR_CLEAR2_OFFSET, INTC_MIR2_MASK);

    return;
}


