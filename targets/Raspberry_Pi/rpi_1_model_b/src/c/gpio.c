/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-01-03
 *      Updated:    2015-05-11
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  gpio.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       Updated and copied to bOS
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "gpio.h"
#include "hardware_system.h"


void gpio_funcsel(const uint32_t offset, const uint32_t bitmap, const uint32_t altfcn)
{
    volatile uint32_t *p = (volatile uint32_t *) (GPIO_BASE + offset);
    uint32_t cnf = *p;
    
    cnf = cnf & ~bitmap;
    cnf = cnf | altfcn;
    *p = cnf;

    return;
}


int32_t gpio_output_high(uint32_t gpio)
{
    uint32_t pin = 1;
    volatile uint32_t *p = NULL;

    if ( !(gpio <= 53) ) {
        //LOG_WARNING(-1);                      Change to an assert!
        return -1;
    }

    if (gpio <= 31) {
        p = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPCLR0);
        pin <<= gpio;
    }
    else if (32 <= gpio) {
        p = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPCLR1);
        gpio -= 32;
        pin <<= gpio;
        gpio += 32;
    }

    *p = pin;
    p = NULL;
    return (int32_t) gpio;
}


int32_t gpio_output_low(uint32_t gpio)
{
    uint32_t pin = 1;
    volatile uint32_t *p = NULL;

    if ( !(gpio <= 53) ) {
        //LOG_WARNING(-1);                      Change to an assert!
        return -1;
    }

    if (gpio <= 31) {
        p = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPSET0);
        pin <<= gpio;
    }
    else if (32 <= gpio) {
        p = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPSET1);
        gpio -= 32;
        pin <<= gpio;
        gpio += 32;
    }

    *p = pin;
    p = NULL;
    return (int32_t) gpio;
}


void gpio_setup_uart(void)
{
    volatile uint32_t *pReg = NULL;
    volatile uint32_t cnt;
    
    
    // GPIO pin 14 (UART TX): configure as alternate function 0
    gpio_funcsel(GPIO_OFFSET_GPFSEL1, GPIO_GPFSEL_GPIO_X4_TRIPLET_BM, GPIO_14_UART_TX);

    //GPIO pin 15 (UART RX): configure as alternate function 0
    gpio_funcsel(GPIO_OFFSET_GPFSEL1, GPIO_GPFSEL_GPIO_X5_TRIPLET_BM, GPIO_15_UART_RX);

    /* The GPIO pins used as TX and RX must be in a floating state, i.e. the
     * pull-up/down should be disabled. According to the reference one must wait
     * 150 clock cycles to provide the required set-up time for the control
     * signal. In addition to this one have to write to respective register to
     * remove the control signal and the clock.
     *
     * OBS: Depending on the optimization level at compilation the wait below might be
     *      optimized away...
     */
    
    pReg = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPPUD);
    *pReg = GPIO_GPPUD_DISABLE_PULL_UP_DOWN;
    for (cnt = 0; cnt < AN150CYCLES; cnt++);
    
    pReg = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPPUDCLK0);
    *pReg = GPIO_GPPUDCLK0_PIN_14 | GPIO_GPPUDCLK0_PIN_15;
    for (cnt = 0; cnt < AN150CYCLES; cnt++);
    
    pReg = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPPUD);
    *pReg = 0;
    
     pReg = (volatile uint32_t *) (GPIO_BASE + GPIO_OFFSET_GPPUDCLK0);
    *pReg = 0;   

    return;
}

