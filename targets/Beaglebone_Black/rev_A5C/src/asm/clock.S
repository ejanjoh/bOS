/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2013-11-07 (github.com/ejanjoh/BeagleBone-Black/source/asm/clock.s)
 *      Updated:    2016-05-13
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  clock.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 12      File copied from Beaglebone Black project and updated
 *                  to fit the need, i.e. get the level 3 SDRAM up and running.
 *
 *      Reference: 
 *          - hardware_system.h
 *          - chapter 2, 7, 8 and 25 (AM335x TRM)
 *
 ******************************************************************************/

#include "hardware_system.h"



        /*********************************************************************** 
         * clock_setup
         *
         * Setup all PLLs and clocks needed by the system.
         *
         * Note: Almost all relavant clocks are initiated by the ROM_code 
         *       earlier in the boot process. However, our attempt is to set up
         *       the PLLs and clocks needed by this system.
         *
         * C prototype: void clock_setup(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_setup
clock_setup:
stmfd   sp!, {fp, lr}

        /* Initiate clock source for UART0, EMIF etc... */
        bl      clock_init_pll

        /* Enforce that Clock Managers used is a up an running... */
        mov     r6, #0x2                                            /* force wake-up */
        
        ldr     r4, =CM_WKUP_BASE
        str     r6, [r4, #CM_WKUP_CLKSTCTRL_OFFSET]

        ldr     r4, =CM_PER_BASE
        str     r6, [r4, #CM_PER_L3_CLKSTCTRL_OFFSET]

        bl      clock_enable_uart0
        bl      clock_enable_emif
        bl      clock_enable_gpio1
        bl      clock_enable_timer2

        ldmfd   sp!, {fp, pc}


        /*********************************************************************** 
         * ClockEnableGPIO0
         *
         * Enable the interface and functional clocks on GPIO0
         *
         * C prototype: void ClockEnableGPIO0(void) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global ClockEnableGPIO0
ClockEnableGPIO0:
        ldr     r0, =CM_WKUP_BASE
        ldr     r1, =0x40002                                        /* functional and interface clocks */
        str     r1, [r0, #CM_WKUP_GPIO0_CLKCTRL_OFFSET]
1:
        ldr     r2, [r0, #CM_WKUP_GPIO0_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     1b
        mov     pc, lr


        /*********************************************************************** 
         * clock_enable_gpio1
         *
         * Enable the interface and functional clocks on GPIO1
         *
         * C prototype: void clock_enable_gpio1(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_enable_gpio1
clock_enable_gpio1:
        movw    r0, #:lower16:CM_PER_BASE
        movt    r0, #:upper16:CM_PER_BASE
        movw    r1, #:lower16:CM_PER_FUNCT_INTRFC_CLCK_MASK         @ functional and interface clocks
        movt    r1, #:upper16:CM_PER_FUNCT_INTRFC_CLCK_MASK
        str     r1, [r0, #CM_PER_GPIO1_CLKCTRL_OFFSET]
1:
        ldr     r2, [r0, #CM_PER_GPIO1_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     1b
        mov     pc, lr


        /*********************************************************************** 
         * clock_enable_uart0
         *
         * Enable the interface and functional clocks on UART0
         *
         * C prototype: void clock_enable_uart0(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_enable_uart0
clock_enable_uart0:
        movw    r0, #:lower16:CM_WKUP_BASE
        movt    r0, #:upper16:CM_WKUP_BASE
        mov     r1, #CM_WKUP_FUNCT_INTRFC_CLCK_MASK
        str     r1, [r0, #CM_WKUP_UART0_CLKCTRL_OFFSET]
1:
        ldr     r2, [r0, #CM_WKUP_UART0_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     1b
        mov     pc, lr



        /*********************************************************************** 
         * clock_enable_emif
         *
         * Enable the interface and functional clocks needed by EMIF
         *
         * C prototype: void clock_enable_emif(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_enable_emif
clock_enable_emif:
        ldr     r0, =CM_PER_BASE
        mov     r1, #0x2                                            /* enable module */
1:
        str     r1, [r0, #CM_PER_EMIF_CLKCTRL_OFFSET]
2:
        ldr     r2, [r0, #CM_PER_EMIF_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     2b
3:
        str     r1, [r0, #CM_PER_L3_CLKCTRL_OFFSET]
4:
        ldr     r2, [r0, #CM_PER_L3_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     4b

        mov     pc, lr


        /***********************************************************************
         * clock_enable_timer2
         *
         * Enable the interface and functional clocks needed by Timer2 and set
         * the clock for a proper timer resolution (41.66 ns per tick, 24 MHz).
         *
         * C prototype: void clock_enable_timer2(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_enable_timer2
clock_enable_timer2:

        @ set up the clock source to timer 2 (source: CLK_M_OSC, 24 MHz)
        movw    r0, #:lower16:CM_DPLL_BASE
        movt    r0, #:upper16:CM_DPLL_BASE
        mov     r1, #CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC_MASK    @ CLK_M_OSC = 24 MHz
        str     r1, [r0, #CM_DPLL_CLKSEL_TIMER2_CLK_OFFSET]

        movw    r0, #:lower16:CM_PER_BASE
        movt    r0, #:upper16:CM_PER_BASE
        mov     r1, #CM_PER_CLKCTRL_ENABLE_MASK
        
        @ interface clock (CM_PER_L4LS_GCLKCTRL_OFFSET)
        str     r1, [r0, #CM_PER_L4LS_CLKCTRL_OFFSET]
10:
        ldr     r2, [r0, #CM_PER_L4LS_CLKCTRL_OFFSET]
        cmp     r2, r1
        bne     10b

        @ functional clock (CM_PER_TIMER2_GCLK_OFFSET)
        str     r1, [r0, #CM_PER_TIMER2_GCLK_OFFSET]
20:
        ldr     r2, [r0, #CM_PER_TIMER2_GCLK_OFFSET]
        cmp     r2, r1
        bne     20b
        
        mov     pc, lr


        /*********************************************************************** 
         * clock_init_pll
         *
         * Setup and init clock needed in the system
         *
         * C prototype: void clock_init_pll(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global clock_init_pll
clock_init_pll:

        base    .req r0             /* the base address */
        val     .req r1             /* the value to be stored */
        tmp     .req r2             /* temporary */

        /* Core PLL Configuration
            - Set CORE_CLKOUTM4 to 200 MHz
            - Set CORE_CLKOUTM5 to 250 MHz
            - Set CORE_CLKOUTM6 to 500 MHz
            ---> CORE_CLKOUTM4/2 eq to 100 MHz
            ---> CORE_CLKOUTM5/2 eq to 125 MHz
            ---> CORE_CLKOUTM5/5 eq to 50 MHz
            ---> CORE_CLKOUTM5/5/10 eq to 5 MHz */
1:
        ldr     base, =CM_WKUP_BASE
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_CORE_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x4
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_CORE_OFFSET]
2:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_CORE_OFFSET]
        and     val, val, #0x100
        cmp     val, #0
        beq     2b
3:
        ldr     val, [base, #CM_WKUP_CLKSEL_DPLL_CORE_OFFSET]
        ldr     tmp, =0x7FFFF
        bic     val, val, tmp
        orr     val, val, #(992 << 8)                               /* M - value */
        orr     val, val, #23                                       /* N - value */
        str     val, [base, #CM_WKUP_CLKSEL_DPLL_CORE_OFFSET]
4:
        ldr     val, [base, #CM_WKUP_DIV_M4_DPLL_CORE_OFFSET]
        bic     val, val, #0x1F
        orr     val, val, #10                                       /* M4 - value */
        str     val, [base, #CM_WKUP_DIV_M4_DPLL_CORE_OFFSET]

        ldr     val, [base, #CM_WKUP_DIV_M5_DPLL_CORE_OFFSET]
        bic     val, val, #0x1F
        orr     val, val, #8                                        /* M5 - value */
        str     val, [base, #CM_WKUP_DIV_M5_DPLL_CORE_OFFSET]

        ldr     val, [base, #CM_WKUP_DIV_M6_DPLL_CORE_OFFSET]
        bic     val, val, #0x1F
        orr     val, val, #4                                        /* M6 - value */
        str     val, [base, #CM_WKUP_DIV_M6_DPLL_CORE_OFFSET]
5:
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_CORE_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x7
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_CORE_OFFSET]
6:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_CORE_OFFSET]
        and     val, val, #0x1
        cmp     val, #0
        beq     6b

        /* Peripheral PLL Configuration
            - Set PER_CLKOUTM2 to 192 MHz
            ---> PER_CLKOUTM2/2 eq to 96 MHz
            ---> PER_CLKOUTM2/4 eq to 48 MHz */
1:
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_PER_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x4
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_PER_OFFSET]
2:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_PER_OFFSET]
        and     val, val, #0x100
        cmp     val, #0
        beq     2b
3:
        ldr     val, [base, #CM_WKUP_CLKSEL_DPLL_PER_OFFSET]
        ldr     tmp, =0xFFFFF
        bic     val, val, tmp
        orr     val, val, #(960 << 8)                               /* M - value */
        orr     val, val, #23                                       /* N - value */
        str     val, [base, #CM_WKUP_CLKSEL_DPLL_PER_OFFSET]
4:
        ldr     val, [base, #CM_WKUP_DIV_M2_DPLL_PER_OFFSET]
        bic     val, val, #0x7F
        orr     val, val, #5                                        /* M2 - value */
        str     val, [base, #CM_WKUP_DIV_M2_DPLL_PER_OFFSET]
5:
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_PER_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x7
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_PER_OFFSET]
6:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_PER_OFFSET]
        and     val, val, #0x1
        cmp     val, #0
        beq     6b

        /* DDR PLL Configuration
            - Set DDR_PLL_CLKOUT to 400 MHz
            ---> DDR_PLL_CLKOUT/2 eq to 200 MHz */
1:
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_DDR_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x4
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_DDR_OFFSET]
2:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_DDR_OFFSET]
        and     val, val, #0x100
        cmp     val, #0
        beq     2b
3:
        ldr     val, [base, #CM_WKUP_CLKSEL_DPLL_DDR_OFFSET]
        ldr     tmp, =0x7FFFF
        bic     val, val, tmp
        /*ldr     tmp, =(266 << 8)*/
        ldr     tmp, =(400 << 8)
        orr     val, val, tmp                                       /* M - value */
        orr     val, val, #23                                       /* N - value */
        str     val, [base, #CM_WKUP_CLKSEL_DPLL_DDR_OFFSET]
4:
        ldr     val, [base, #CM_WKUP_DIV_M2_DPLL_DDR_OFFSET]
        bic     val, val, #0x1F
        orr     val, val, #1                                        /* M2 - value */
        str     val, [base, #CM_WKUP_DIV_M2_DPLL_DDR_OFFSET]
5:
        ldr     val, [base, #CM_WKUP_CLKMODE_DPLL_DDR_OFFSET]
        bic     val, val, #0x7
        orr     val, val, #0x7
        str     val, [base, #CM_WKUP_CLKMODE_DPLL_DDR_OFFSET]
6:
        ldr     val, [base, #CM_WKUP_IDLEST_DPLL_DDR_OFFSET]
        and     val, val, #0x1
        cmp     val, #0
        beq     6b

        .unreq  base
        .unreq  val
        .unreq  tmp

        mov     pc, lr




