/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-20 (ver 5)
 *      Updated:    2016-05-19 (ver 12)
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  hardware_system.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 5       File created
 *      ver 7       Define endings has been changed
 *                  Added support to handle context switch
 *      ver 12      Updated to handle DDR3 SDRAM (level 3 RAM)
 *
 *
 *      Reference:
 *	        1.  TI Reference Manual AM335x Cortex A8 Microprocessors
 *          2.  ARM ARMv7
 *          3.  BeagleBone Black System Reference Manual
 *          4.  TI SPRS717H –OCTOBER 2011–REVISED MAY 2015, 
 *              AM335x Sitara™ Processors
 *
 *      NOTE 1: This file is used as header file both in C-files and in assembly
 *              files so keep the content to regular defines.
 *
 ******************************************************************************/
 
#ifndef HARDWARE_SYSTEM_H_
#define HARDWARE_SYSTEM_H_


// Metadata
#define     ARM32
#define     ARMv7
#define     BBB
#define     ARM_SYS32_MODE_ONLY

// Access hardware registers, in C only...
#define GET32(addr) (*(volatile unsigned int *) (addr))
#define SET32(addr, val) (*((volatile unsigned int *) (addr)) = ((unsigned int) (val)))


// define _endings using registers
// _REG                 - the address to a hardware register
// _BASE                - the base address to a hardware register
// _OFFSET              - the offset to the base address in a hardware register

// define _endings that is not registers
// _MASK                - the ones defining the bit mask to manipulate hardware registers
// _PAT                 - a pattern or number to be written to a hardware register



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

#define     SPSR_CREATE_PCB                     0x140           // Used to create the PCB for every process (check up!)

// System addresses
#define     LOAD_ADDR_UNDEF_INSTR_EXC_REG       0x4030CE24      // Jump address undefined instruction
#define     LOAD_ADDR_SVC_EXC_REG               0x4030CE28      // Jump address supervisor call
#define     LOAD_ADDR_PREF_ABRT_EXC_REG         0x4030CE2C      // Jump address prefetch abort
#define     LOAD_ADDR_DATA_ABRT_EXC_REG         0x4030CE30      // Jump address data abort
#define     LOAD_ADDR_IRQ_INTR_REG              0x4030CE38      // Jump address irq
#define     LOAD_ADDR_FIQ_INTR_REG              0x4030CE3C      // Jump address fiq
#define     DDR3_SDRAM_START_ADDR               0x80000000      // DDR3 SDRAM 512 M start address
#define     DDR3_SDRAM_END_ADDR                 0x9FFFFFFF      // DDR3 SDRAM 512 M end address

#define     CPSR_SYSTEM_MODE_MASK               0x1F            // CPSR M[4:0]
#define     CPSR_IRQ_MASK                       0x80            // Processor Status Register - IRQ bit
#define     CPSR_FIQ_MASK                       0x40            // Processor Status Register - FIQ bit

#define     STACK_WATERMARK_PAT                 0xFF00FF00
#define     HEAP_WATERMARK_PAT                  0xFF00FF00


// Peripherals...

// Interrupt Control Module (see chapter 6 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     INTC_BASE                           0x48200000
#define     INTC_SIR_IRQ_OFFSET                 0x40
#define     INTC_CONTROL_OFFSET                 0x48
#define     INTC_MIR_CLEAR2_OFFSET              0xC8
#define     INTC_ILR68_OFFSET                   0x210                   // TIMER2
#define     INTC_ILR72_OFFSET                   0x220                   // UART0

#define     INTC_FIQ_IRQ_MASK                   0x01
#define     INTC_MIR2_MASK                      (0x01 << 4 | 0x01 << 8) // TIMER2 and UART0
#define     INTC_ACTIVEIRQ_MASK                 0x7F
#define     INTC_NEW_IRQ_AGR_MASK               0x01

#define     INTC_INT_NUMBER_TIMER2_PAT          68                      // the interrupt number for timer 2 
                                                                        // (used as context switch)
#define     INTC_INT_NUMBER_UART0_PAT           72                      // the interrupt number for UART0 (RHR)
#define     INTC_PRIO_0_PAT                     0x00                    // highest irq prio


// Clock Module registers (see chapter 8 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     CM_PER_BASE                         0x44E00000      // Clock Module Peripheral Registers
#define     CM_WKUP_BASE                        0x44E00400      // Clock Module Wakeup Registers
#define     CM_DPLL_BASE                        0x44E00500      // Clock Module PLL Registers

#define     CM_PER_L3_CLKSTCTRL_OFFSET          0x0C
#define     CM_PER_EMIF_CLKCTRL_OFFSET          0x28
#define     CM_PER_L4LS_CLKCTRL_OFFSET          0x60
#define     CM_PER_TIMER2_GCLK_OFFSET           0x70
#define     CM_PER_GPIO1_CLKCTRL_OFFSET         0xAC
#define     CM_PER_L3_CLKCTRL_OFFSET            0xE0

#define     CM_WKUP_CLKSTCTRL_OFFSET            0x00
#define     CM_WKUP_GPIO0_CLKCTRL_OFFSET        0x08
#define     CM_WKUP_IDLEST_DPLL_DDR_OFFSET      0x34
#define     CM_WKUP_CLKSEL_DPLL_DDR_OFFSET      0x40
#define     CM_WKUP_IDLEST_DPLL_CORE_OFFSET     0x5C
#define     CM_WKUP_CLKSEL_DPLL_CORE_OFFSET     0x68
#define     CM_WKUP_IDLEST_DPLL_PER_OFFSET      0x70
#define     CM_WKUP_DIV_M4_DPLL_CORE_OFFSET     0x80
#define     CM_WKUP_DIV_M5_DPLL_CORE_OFFSET     0x84
#define     CM_WKUP_CLKMODE_DPLL_PER_OFFSET     0x8C
#define     CM_WKUP_CLKMODE_DPLL_CORE_OFFSET    0x90
#define     CM_WKUP_CLKMODE_DPLL_DDR_OFFSET     0x94
#define     CM_WKUP_CLKSEL_DPLL_PER_OFFSET      0x9C
#define     CM_WKUP_DIV_M2_DPLL_DDR_OFFSET      0xA0
#define     CM_WKUP_DIV_M2_DPLL_PER_OFFSET      0xAC
#define     CM_WKUP_UART0_CLKCTRL_OFFSET        0xB4
#define     CM_WKUP_DIV_M6_DPLL_CORE_OFFSET     0xD8

#define     CM_DPLL_CLKSEL_TIMER2_CLK_OFFSET    0x08

#define     CM_PER_FUNCT_INTRFC_CLCK_MASK       0x40002
#define     CM_PER_CLKCTRL_ENABLE_MASK          0x02
#define     CM_WKUP_FUNCT_INTRFC_CLCK_MASK      0x02
#define     CM_DPLL_CLKSEL_TIMER2_CLK_CLKSEL_CLK_M_OSC_MASK     0x01


// Control Module (see chapter 9 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     CTRL_MODE_BASE                      0x44E10000
#define     CTRL_MODE_CONF_UART0_RXD_OFFSET     0x970
#define     CTRL_MODE_CONF_UART0_TXD_OFFSET     0x974

#define     CTRL_MODE_PULLUDEN_MASK             (1 << 3)    // Pad pullup/pulldown enable (1 = pullup/pulldown disabled)
#define     CTRL_MODE_PULLTYPESEL_MASK          (1 << 4)    // Pad pullup/pulldown type selection (1 = Pullup selected)
#define     CTRL_MODE_RXACTIVE_MASK             (1 << 5)    // Input enable value for the Pad. Set to 0 for output only. 
                                                            // Set to 1 for input or output.

#define     CTRL_MODE_UART0_RX_TX_MUXMODE_PAT   0x00        // Mux mode setting for UART0 RX and TX


// UART registers ( see chapter 19 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     UART0_BASE                          0x44E09000
#define     UART1_BASE                          0x48022000
#define     UART2_BASE                          0x48024000
#define     UART3_BASE                          0x481A6000
#define     UART4_BASE                          0x481A8000
#define     UART5_BASE                          0x481AA000
#define     UART_THR_OFFSET                     0x00
#define     UART_RHR_OFFSET                     0x00
#define     UART_DLL_OFFSET                     0x00
#define     UART_DLH_OFFSET                     0x04
#define     UART_IER_UART_OFFSET                0x04
#define     UART_EFR_OFFSET                     0x08
#define     UART_FCR_OFFSET                     0x08
#define     UART_IIR_OFFSET                     0x08
#define     UART_LCR_OFFSET                     0x0C
#define     UART_MCR_OFFSET                     0x10
#define     UART_LSR_UART_OFFSET                0x14
#define     UART_MDR1_OFFSET                    0x20
#define     UART_SYSC_OFFSET                    0x54
#define     UART_SYSS_OFFSET                    0x58

#define     UART_SYSC_SOFTRESET_MASK            0x02
#define     UART_SYSS_RESETDONE_MASK            0x01
#define     UART_REG_CONFIG_MODE_A_MASK         0x80
#define     UART_REG_CONFIG_MODE_B_MASK         0xBF
#define     UART_REGISTER_OPERATIONAL_MODE_MASK 0x00
#define     UART_NO_DMA_NOR_FIFO_MASK           0x00
#define     UART_MDR1_MODE_SELECT_MASK          0x07
#define     UART_IER_CLEAR_MASK                 0x00
#define     UART_DLL_CLOCK_LSB_MASK             0x1A
#define     UART_DLH_CLOCK_MSB_MASK             0x00
#define     UART_IER_INTERRUPT_DISABLED_MASK    0x00
#define     UART_IER_INTERRUPT_RHR_ENABLED      0x01
#define     UART_LCR_8_BIT_CHAR_LENGTH_MASK     0x03
#define     UART_LCR_1_STOP_BIT_MASK            (0x01 << 2)
#define     UART_EFR_ENHANCED_EN_MASK           (0x1 << 4)
#define     UART_MCR_TCR_TLR_MASK               (0x1 << 6)
#define     UART_LSR_UART_RXFIFOE_MASK          0x01
#define     UART_LSR_UART_TXFIFOE_MASK          0x20
#define     UART_IIR_IT_PEND_MASK               0x01                // = 0x01 No interrupt pending
#define     UARI_IIR_IT_TYPE_MASK               0x3E
#define     UART_IIR_IT_RHR_PEND_MASK           0x04


// Timer registers (see chapter 20 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     DM_TIMER2_BASE                      0x48040000
#define     TIMER_IRQSTATUS_OFFSET              0x28                // Pending and clear interrupt
#define     TIMER_IRQENABLE_SET_OFFSET          0x2C                // Enable interrupt
#define     TIMER_TCLR_OFFSET                   0x38                // Timer Control Register
#define     TIMER_TCRR_OFFSET                   0x3C                // Timer Counter Register
#define     TIMER_TMAR_OFFSET                   0x4C                // Timer match register

#define     TIMER_IRQ_ENABLE_FOR_MATCH_MASK     0x01
#define     TCLR_START_VALUE_MASK               (0x01 << 0 | 0x01 << 6)
#define     TIMER_IRQSTATUS_MAT_IT_FLAG_MASK    0x01                // shouldn't we let this one be 0x07 instead?

#define     SYS_TIMER_INTR_INTERVAL_PAT         0x3B00              // The time (in ticks) between two interrupts
#define     TCRR_INIT_VALUE_PAT                 0x00


// GPIO (see chapter 25 in TI Reference Manual AM335x Cortex A8 Microprocessors)
#define     GPIO0_BASE                          0x44E07000
#define     GPIO1_BASE                          0x4804C000
#define     GPIO2_BASE                          0x481AC000
#define     GPIO3_BASE                          0x481AE000
#define     GPIO_OE_OFFSET                      0x134
#define     GPIO_CLEARDATAOUT_OFFSET            0x190
#define     GPIO_SETDATAOUT_OFFSET              0x194

#define     GPIO1_21_USR_LED_0_MASK             (1 << 21)
#define     GPIO1_22_USR_LED_1_MASK             (1 << 22)
#define     GPIO1_23_USR_LED_2_MASK             (1 << 23)
#define     GPIO1_24_USR_LED_3_MASK             (1 << 24)
#define     GPIO1_ALL_USR_LEDS_MASK             ((1 << 21) | (1 << 22) | (1 << 23) | (1 << 24))





#endif /* HARDWARE_SYSTEM_H_ */