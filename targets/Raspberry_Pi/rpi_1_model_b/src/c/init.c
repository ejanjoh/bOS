/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-12
 *      Updated:    2015-05-30
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
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


extern uint32_t _start;



void init(void)
{
    printf(100, "System started at: %p\r\n", &_start);

    /*
     *
     * Add initialization of PCB block etc...
     *
     */
     
     // Enable interrupts
     *((volatile uint32_t *) (INTR_BASE + INTR_ENABLE_IRQ_1)) |= INTR_ENABLE_IRQ_1_TIMER_BM;      // ARM timer
     
	// Start timer interrupt and add a delay time until the ordinary timeout is used...   (move to timer.c if needed)
    *((volatile uint32_t *) (SYSTEM_TIMER_BASE + SYSTEM_TIMER_C1)) = 
            *((volatile uint32_t *) (SYSTEM_TIMER_BASE + SYSTEM_TIMER_CLO)) + SYS_TIMER_INIT_DELAY;
    *((volatile uint32_t *) (SYSTEM_TIMER_BASE + SYSTEM_TIMER_CS)) = SYS_TIMER_CS_M1;

    return;
}


