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


extern uint32_t _start;
extern void init_context_switch_timer2(void);

void init(void)
{
    printf(100, "System started at: %p\r\n", &_start);

    /*
     *
     * Add initialization of PCB block etc...
     *
     */

    // Set up the timer needed for the interrups
    init_context_switch_timer2();

    // Set up the interrupt controller
    // - Assign priority and select IRQ
    SET32(INTC_BASE + INTC_ILR68_OFFSET, (~INTC_FIQ_IRQ_MASK & 0x00) | INTC_PRIO_0_PAT);
    // - Set up the interrupt mask
    SET32(INTC_BASE + INTC_MIR_CLEAR2_OFFSET, INTC_MIR2_MASK);

    return;
}


