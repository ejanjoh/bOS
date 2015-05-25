/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-20 (ver 5)
 *      Updated:    
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  hardware_system.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File created
 *
 *
 *      Reference:
 *	        1.  TI Reference Manual AM335x Cortex A8 Microprocessors
 *          2.  ARM ARMv7
 *          3.  BeagleBone Black System Reference Manual
 *
 *      NOTE 1: This file is used as header file both in C-files and in assembly
 *              files so keep the content to regular defines.
 *
 ******************************************************************************/
 
#ifndef HARDWARE_SYSTEM_H_
#define HARDWARE_SYSTEM_H_

/*
 *  ARM Modes              Encoding    Security state      Privilege level
 *  ======================================================================
 *  User            (usr)   10000       Both                PL0
 *  FIQ             (fiq)   10001       Both                PL1
 *  IRQ             (irq)   10010       Both                PL1
 *  Supervisor      (svc)   10011       Both                PL1
 *  Monitor         (mon)   10110       Secure only         PL1
 *  Abort           (abt)   10111       Both                PL1
 *  Hyp             (hyp)   11010       Non-secure          PL2
 *  Undef           (und)   11011       Both                PL1
 *  System          (sys)   11111       Both                PL1
 */
 
 
/* Default interrupt vector table on BBB
 *
 * Address    Exception       Content
 * ===============================================
 * 4030CE00h  Reserved        Reserved
 * 4030CE04h  Undefined       PC = [4030CE24h]
 * 4030CE08h  SVC             PC = [4030CE28h]
 * 4030CE0Ch  Prefetch abort  PC = [4030CE2Ch]
 * 4030CE10h  Data abort      PC = [4030CE30h]
 * 4030CE14h  Unused          PC = [4030CE34h]
 * 4030CE18h  IRQ             PC = [4030CE38h]
 * 4030CE1Ch  FIQ             PC = [4030CE3Ch]
 *
 * 4030CE20h  Reserved        20090h
 * 4030CE24h  Undefined       20080h
 * 4030CE28h  SVC             20084h
 * 4030CE2Ch  Prefetch abort  Address of default pre-fetch abort handler
 * 4030CE30h  Data abort      Address of default data abort handler
 * 4030CE34h  Unused          20090h
 * 4030CE38h  IRQ             Address of default IRQ handler
 * 4030CE3Ch  FIQ             20098h
 *
 *
 * ARM exception v.s. mode mapping
 *
 * ARM Exception              Mode            CPSR interrupt mask
 * ===========================================================
 * Reset                      Supervisor      F = 1, I = 1
 * Undefined Instruction      Undef                  I = 1
 * Supervisor Call            Supervisor             I = 1
 * Prefetch Abort             Abort                  I = 1
 * Data Abort                 Abort                  I = 1
 * Not Used                   HYP             -      -
 * IRQ Interrupt              IRQ                    I = 1
 * FIQ Interrupt              FIQ             F = 1, I = 1
 */

// Bit masks and patterns to be moved to local files if possible...

// ARM processor (ARM7) system modes:
// - Operating modes...
#define     USR32_MODE                          0x10            // User mode
#define     SYS32_MODE                          0x1F            // System mode
// - Exception modes...
#define     FIQ32_MODE                          0x11            // Fast interrupt mode
#define     IRQ32_MODE                          0x12            // Interrupt mode
#define     SVS32_MODE                          0x13            // Supervisor mode
#define     ABT32_MODE                          0x17            // Abort mode
#define     UND32_MODE                          0x1B            // Undefined mode

// System addresses
#define     LOAD_ADDR_UNDEF_INSTR_EXC           0x4030CE24      // Jump address undefined instruction
#define     LOAD_ADDR_SVC_EXC                   0x4030CE28      // Jump address supervisor call
#define     LOAD_ADDR_PREF_ABRT_EXC             0x4030CE2C      // Jump address prefetch abort
#define     LOAD_ADDR_DATA_ABRT_EXC             0x4030CE30      // Jump address data abort
#define     LOAD_ADDR_IRQ_INTR                  0x4030CE38      // Jump address irq
#define     LOAD_ADDR_FIQ_INTR                  0x4030CE3C      // Jump address fiq

// Bit Masks
#define     CPSR_SYSTEM_MODE_BM                 0x1F            // CPSR M[4:0]

#define     CTRL_MODE_PULLUP_PULLDOWN_BM        (1 << 3)        // Control Module pullup/pulldown
#define     CTRL_MODE_RECEIVER_BM               (1 << 5)        // Control Module receiver

#define     GPIO1_21_USR_LED_0_BM               (1 << 21)
#define     GPIO1_22_USR_LED_1_BM               (1 << 22)
#define     GPIO1_23_USR_LED_2_BM               (1 << 23)
#define     GPIO1_24_USR_LED_3_BM               (1 << 24)
#define     GPIO1_ALL_USR_LEDS_BM               ((1 << 21) | (1 << 22) | (1 << 23) | (1 << 24))

#define     CM_PER_FUNCT_INTRFC_CLCK_BM         0x40002
#define     WKUP_FUNCT_INTRFC_CLCK_BM           0x2

#define     UART_SYSC_SOFTRESET_BM              0x02
#define     UART_SYSS_RESETDONE_BM              0x01
#define     UART_REG_CONFIG_MODE_A_BM           0x80
#define     UART_REG_CONFIG_MODE_B_BM           0xBF
#define     UART_REGISTER_OPERATIONAL_MODE_BM   0x00
#define     UART_NO_DMA_NOR_FIFO_BM             0x00
#define     UART_MDR1_MODE_SELECT_BM            0x07
#define     UART_IER_CLEAR_BM                   0x00
#define     UART_DLL_CLOCK_LSB_BM               0x1A
#define     UART_DLH_CLOCK_MSB_BM               0x00
#define     UART_IER_INTERRUPT_DISABLED_BM      0x00
#define     UART_LCR_8_BIT_CHAR_LENGTH_BM       0x03
#define     UART_LCR_1_STOP_BIT_BM              (0x01 << 2)
#define     UART_EFR_ENHANCED_EN_BM             (0x1 << 4)
#define     UART_MCR_TCR_TLR_BM                 (0x1 << 6)
#define     UART_LSR_UART_RXFIFOE               0x01
#define     UART_LSR_UART_TXFIFOE               0x20

// Patterns
#define     STACK_WATERMARK                     0xFF00FF00
#define     HEAP_WATERMARK                      0xFF00FF00

// Peripherals

// Control Module (see chapter 9 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     CTRL_MOD_BASE                       0x44E10000
#define     CTRL_MOD_OFFSET_CONF_UART0_RXD      0x970
#define     CTRL_MOD_OFFSET_CONF_UART0_TXD      0x974

// GPIO (see chapter 25 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     GPIO0_BASE                          0x44E07000
#define     GPIO1_BASE                          0x4804C000
#define     GPIO2_BASE                          0x481AC000
#define     GPIO3_BASE                          0x481AE000
#define     GPIO_OE                             0x134
#define     GPIO_CLEARDATAOUT                   0x190
#define     GPIO_SETDATAOUT                     0x194

// Clock Module registers (see chapter 8 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     CM_PER_BASE                         0x44E00000      @ Clock Module Peripheral Registers
#define     CM_WKUP_BASE                        0x44E00400      @ Clock Module Wakeup Registers
#define     CM_PER_GPIO1_CLKCTRL                0xAC
#define     CM_WKUP_UART0_CLKCTRL               0xB4

// UART registers ( see chapter 19 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     UART0_BASE                          0x44E09000

#define     UART_THR                            0x00
#define     UART_RHR                            0x00
#define     UART_DLL                            0x00
#define     UART_DLH                            0x04
#define     UART_IER_UART                       0x04
#define     UART_EFR                            0x08
#define     UART_FCR                            0x08
#define     UART_LCR                            0x0C
#define     UART_MCR                            0x10
#define     UART_LSR_UART                       0x14
#define     UART_MDR1                           0x20
#define     UART_SYSC                           0x54
#define     UART_SYSS                           0x58



#endif /* HARDWARE_SYSTEM_H_ */