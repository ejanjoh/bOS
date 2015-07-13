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
    uint32_t cnf = GET32(GPIO_BASE + offset);
    cnf = cnf & ~bitmap;
    cnf = cnf | altfcn;
    SET32(GPIO_BASE + offset, cnf);
    return;
}


int32_t gpio_output_high(uint32_t gpio)
{
    uint32_t pin = 1;

    if ( !(gpio <= 53) ) {
        return -1;
    }

    if (gpio <= 31) {
        pin <<= gpio;
        SET32(GPIO_BASE + GPIO_GPCLR0_OFFSET, pin);
    }
    else if (32 <= gpio) {
        gpio -= 32;
        pin <<= gpio;
        gpio += 32;
        SET32(GPIO_BASE + GPIO_GPCLR1_OFFSET, pin);
    }

    return (int32_t) gpio;
}


int32_t gpio_output_low(uint32_t gpio)
{
    uint32_t pin = 1;

    if ( !(gpio <= 53) ) {
        return -1;
    }

    if (gpio <= 31) {
        pin <<= gpio;
        SET32(GPIO_BASE + GPIO_GPSET0_OFFSET, pin);
    }
    else if (32 <= gpio) {
        gpio -= 32;
        pin <<= gpio;
        gpio += 32;
        SET32(GPIO_BASE + GPIO_GPSET1_OFFSET, pin);
    }

    return (int32_t) gpio;
}


void gpio_setup_uart(void)
{
    volatile uint32_t cnt;   

    // GPIO pin 14 (UART TX): configure as alternate function 0
    gpio_funcsel(GPIO_GPFSEL1_OFFSET, GPIO_GPFSEL_GPIO_X4_TRIPLET_MASK, GPIO_14_UART_TX_MASK);

    //GPIO pin 15 (UART RX): configure as alternate function 0
    gpio_funcsel(GPIO_GPFSEL1_OFFSET, GPIO_GPFSEL_GPIO_X5_TRIPLET_MASK, GPIO_15_UART_RX_MASK);

    /* The GPIO pins used as TX and RX must be in a floating state, i.e. the
     * pull-up/down should be disabled. According to the reference one must wait
     * 150 clock cycles to provide the required set-up time for the control
     * signal. In addition to this one have to write to respective register to
     * remove the control signal and the clock.
     *
     * OBS: Depending on the optimization level at compilation the wait below might be
     *      optimized away...
     */
    
    SET32(GPIO_BASE + GPIO_GPPUD_OFFSET, GPIO_GPPUD_DISABLE_PULL_UP_DOWN_PAT);
    for (cnt = 0; cnt < AN150CYCLES_PAT; cnt++);

    SET32(GPIO_BASE + GPIO_GPPUDCLK0_OFFSET, GPIO_GPPUDCLK0_PIN_14_MASK | GPIO_GPPUDCLK0_PIN_15_MASK);
    for (cnt = 0; cnt < AN150CYCLES_PAT; cnt++);

    SET32(GPIO_BASE + GPIO_GPPUD_OFFSET, 0);
    SET32(GPIO_BASE + GPIO_GPPUDCLK0_OFFSET, 0);
    return;
}

