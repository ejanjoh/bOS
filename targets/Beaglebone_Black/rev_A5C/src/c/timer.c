/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-06-04
 *      Updated:
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  timer.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 7       Created
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "io.h"
#include "hardware_system.h"


//static void irq_test(void);


void init_context_switch_timer2(void)
{

    /* Set the pre-scale value based on the frequency 24 MHz (41.66 ns ticks) defined in clock_enable_timer2
     *
     *      bit     Field       value   comment
     *      ========================================================
     *      0       ST          0x1     Start timer
     *      1       AR          0x0     One shot
     *      2-4     PTV         ignore ￼Pre-scale clock Timer value   x |
     *      5       PRE         0x0     Prescaler enable - don´t      0 | ==> divisor (ps) = 1 ==> 41,66... ns ticks
     *      6       CE          0x1     Compare Enable
     *      7-31                        Ignore
     *
     *      ==> TCLR_INIT_VALUE_MASK ( = 0x41 = (0x01 << 0 | 0x01 << 6))
     */

    // Enable the interrupts and set the timer match register
    SET32(DM_TIMER2_BASE + TIMER_IRQENABLE_SET_OFFSET, TIMER_IRQ_ENABLE_FOR_MATCH_MASK);
    SET32(DM_TIMER2_BASE + TIMER_TMAR_OFFSET, SYS_TIMER_INTR_INTERVAL_PAT);

    // Set the timer to the start value and start the timer...
    SET32(DM_TIMER2_BASE + TIMER_TCLR_OFFSET, TCLR_START_VALUE_MASK);
    SET32(DM_TIMER2_BASE + TIMER_TCRR_OFFSET, TCRR_INIT_VALUE_PAT);

    return;
}


// just a temporary function to test that the interrupt works as intended. To be removed and replaced by a scheduler!
/*void update_context_switch_timer2(void)
{
    //_puts("System timer has expired - system has been interrupted... \n", 100);

    // clear the interrupt
    SET32(DM_TIMER2_BASE + TIMER_IRQSTATUS_OFFSET, TIMER_IRQSTATUS_MAT_IT_FLAG_MASK);

    // Set the timer counter to the start value
    SET32(DM_TIMER2_BASE + TIMER_TCRR_OFFSET, TCRR_INIT_VALUE_PAT);

    return;
}*/



