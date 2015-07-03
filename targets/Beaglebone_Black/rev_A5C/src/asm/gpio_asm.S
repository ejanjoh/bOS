/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-11-07
 *      Updated:    2015-05-21
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  gpio_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File copied and updated
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"


        /*********************************************************************** 
         * gpio_enable
         *
         * Enable the GPIOs needed by the system
         *
         * C prototype: void gpio_enable(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global gpio_enable
gpio_enable:
        movw    r0, #:lower16:GPIO1_BASE
        movt    r0, #:upper16:GPIO1_BASE
        
        @ --- GPIO1_21 to GPIO1_24 - the user LEDs usr0 to usr3
        ldr     r1, [r0, #GPIO_OE_OFFSET]
        bic     r1, r1, #GPIO1_ALL_USR_LEDS_MASK
        str     r1, [r0, #GPIO_OE_OFFSET]

        mov     pc, lr


        /*********************************************************************** 
         *  gpio_usrled_on
         *
         *  Turn on one or more user leds
         *
         *  in: r0:
         *      - GPIO1_21_USR_LED_0_MASK
         *      - GPIO1_22_USR_LED_1_MASK
         *      - GPIO1_23_USR_LED_2_MASK
         *      - GPIO1_24_USR_LED_3_MASK
         *      - GPIO1_ALL_USR_LEDS_MASK
         *
         * C prototype: void gpio_usrled_on(uint32_t led)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global gpio_usrled_on
gpio_usrled_on:
        movw    r1, #:lower16:GPIO1_BASE
        movt    r1, #:upper16:GPIO1_BASE

        mov     r2, #0xF
        lsl     r2, r2, #21
        and     r0, r0, r2
        cmp     r0, #0x0
        beq     100f
        
        str     r0, [r1, #GPIO_SETDATAOUT_OFFSET]
100:
        mov     pc, lr


        /*********************************************************************** 
         *  gpio_usrled_off
         *
         *  Turn off one or more user leds
         *
         *  in: r0:
         *      - GPIO1_21_USR_LED_0_BM
         *      - GPIO1_22_USR_LED_1_BM
         *      - GPIO1_23_USR_LED_2_BM
         *      - GPIO1_24_USR_LED_3_BM
         *      - GPIO1_ALL_USR_LEDS_BM
         *
         * C prototype: void gpio_usrled_off(uint32_t led)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global gpio_usrled_off
gpio_usrled_off:
        movw    r1, #:lower16:GPIO1_BASE
        movt    r1, #:upper16:GPIO1_BASE

        mov     r2, #0xF
        lsl     r2, r2, #21
        and     r0, r0, r2
        cmp     r0, #0x0
        beq     100f
        
        str     r0, [r1, #GPIO_CLEARDATAOUT_OFFSET]
100:
        mov     pc, lr


        /*********************************************************************** 
         *  gpio1_x_setlow
         *
         *  Set GPIO1_x low
         *
         *  in: r0: set GPIO1_x low
         *
         * C prototype: void gpio1_x_setlow(const uint_t gpio1_x)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global gpio1_x_setlow
gpio1_x_setlow:
        movw    r1, #:lower16:GPIO1_BASE
        movt    r1, #:upper16:GPIO1_BASE
        mov     r2, #1
        lsl     r2, r2, r0
        str     r2, [r1, #GPIO_CLEARDATAOUT_OFFSET]

        mov     pc, lr


        /*********************************************************************** 
         *  gpio1_x_sethigh
         *
         *  Set GPIO1_x high
         *
         *  in: r0: set GPIO1_x high
         *
         * C prototype: void gpio1_x_sethigh(const uint_t gpio1_x)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global gpio1_x_sethigh
gpio1_x_sethigh:
        movw    r1, #:lower16:GPIO1_BASE
        movt    r1, #:upper16:GPIO1_BASE
        mov     r2, #1
        lsl     r2, r2, r0
        str     r2, [r1, #GPIO_SETDATAOUT_OFFSET]

        mov     pc, lr



