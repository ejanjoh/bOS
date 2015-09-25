/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  init.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       Updated
 *      ver 10      Enabled UART Rx handling.
 *                  Added new processes.
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "hardware_system.h"
#include "io.h"
#include "process_control.h"
#include "sys_def.h"


extern void cli_io_init(void);
extern int32_t _printf(const uint32_t len, const char *format, ...);

extern uint32_t _start;                 // from linker script
extern uint32_t __stack_sys_top;        // from linker script

extern void main(void);                 // found in main.c
extern void procA(void);                // found in process_control.c
extern void procB(void);                // found in process_control.c
extern void procC(void);                // found in process_control.c
extern void procD(void);                // found in process_control.c
extern void cli_io_in_proc(void);       // found in cli.c
extern void cli_io_out_proc(void);      // found in cli.c


// Respective process stack top and size, the base for this define are to be found in
// the linker script kernel.ld
#define STACK_SIZE 0x1000               // sys_stack_size/num_sys_proc, see linker script
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



void init(void)
{
    _printf(100, "System started at: %p\n", &_start);
    
    // Create the PCB
    for (uint32_t pcb = 0; pcb < NUMB_PROC; pcb++) proc_ctrl_create_pcb(init_pcb + pcb);
    
    // Init the "stdio" buffers used in the system
    cli_io_init();
    
    // Enable UART interrupts
    SET32(INTR_BASE + INTR_ENABLE_IRQ_2_OFFSET, 
          GET32(INTR_BASE + INTR_ENABLE_IRQ_2_OFFSET) | INTR_UART_RX_PEND_MASK);

    // Enable interrupts (ARM timer)
    SET32(INTR_BASE + INTR_ENABLE_IRQ_1_OFFSET, 
          GET32(INTR_BASE + INTR_ENABLE_IRQ_1_OFFSET) | INTR_ENABLE_IRQ_1_TIMER_MASK);

	// Start timer interrupt and add a delay time until the ordinary timeout is used...   (move to timer.c if needed)
    SET32(SYSTEM_TIMER_BASE + SYSTEM_TIMER_C1_OFFSET, 
          GET32(SYSTEM_TIMER_BASE + SYSTEM_TIMER_CLO_OFFSET) + SYS_TIMER_INIT_DELAY_PAT); 
    SET32(SYSTEM_TIMER_BASE + SYSTEM_TIMER_CS_OFFSET, SYS_TIMER_CS_M1_MASK);

    return;
}


