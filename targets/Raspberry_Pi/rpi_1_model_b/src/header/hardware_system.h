/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-07 (ver 3)
 *      Updated:    
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  hardware_system.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *
 *
 *      Reference:
 *	        1. ARM 1176JZF-S technical reference manual
 *          2. ARM Architecture Reference Manual (ARM ARM)
 *          3. BroadCom BMC2835 ARM Peripherals
 *          4. ARM PrimeCell UART (PL011) revision r1p4 
 *
 *      NOTE 1: This file is used as header file both in C-files and in assembly
 *              files so keep the content to regular defines.
 *
 ******************************************************************************/

#ifndef HARDWARE_SYSTEM_H_
#define HARDWARE_SYSTEM_H_

// ARM processor (arm1176jzf-s) system modes
// Operating modes...
#define     USR32_MODE                          0x10            // User mode
#define     SYS32_MODE                          0x1F            // System mode
// Exception modes...
#define     SVS32_MODE                          0x13            // Supervisor mode
#define     ABT32_MODE                          0x17            // Abort mode
#define     UND32_MODE                          0x1B            // Undefined mode
#define     IRQ32_MODE                          0x12            // Interrupt mode
#define     FIQ32_MODE                          0x11            // Fast interrupt mode


// System addresses
#define     SYSTEM_START_ADDR                   0x00008000      // The system start address after GPU boot up
#define     INTERRUPT_VECTOR_TABLE_ADDR         0x00000000      // The address of the reset vector


// BIT MASKS
#define     CPSR_SYSTEM_MODE_BM                 0x1F            // CPSR M[4:0]

#define     GPIO_GPFSEL_GPIO_X0_TRIPLET_BM      0x7
#define     GPIO_GPFSEL_GPIO_X1_TRIPLET_BM      (0x7 << (3*1))
#define     GPIO_GPFSEL_GPIO_X2_TRIPLET_BM      (0x7 << (3*2))
#define     GPIO_GPFSEL_GPIO_X3_TRIPLET_BM      (0x7 << (3*3))
#define     GPIO_GPFSEL_GPIO_X4_TRIPLET_BM      (0x7 << (3*4))
#define     GPIO_GPFSEL_GPIO_X5_TRIPLET_BM      (0x7 << (3*5))
#define     GPIO_GPFSEL_GPIO_X6_TRIPLET_BM      (0x7 << (3*6))
#define     GPIO_GPFSEL_GPIO_X7_TRIPLET_BM      (0x7 << (3*7))
#define     GPIO_GPFSEL_GPIO_X8_TRIPLET_BM      (0x7 << (3*8))
#define     GPIO_GPFSEL_GPIO_X9_TRIPLET_BM      (0x7 << (3*9))

#define     UART_FR_RX_FIFO_EMPTY_BM            (0x1 << 4)
#define     UART_FR_TRANSMIT_FIFO_FULL_BM       (0x1 << 5)

// Patterns
#define     STACK_WATERMARK                     0xFF00FF00
#define     HEAP_WATERMARK                      0xFF00FF00
#define     AN150CYCLES                         150

#define     GPIO_14_UART_TX                     (0x04 << 12)    // GPIO pin 14 (UART TX) config. as altern. funct. 0
#define     GPIO_15_UART_RX                     (0x04 << 15)    // GPIO pin 15 (UART RX) config. as altern. funct. 0
#define     GPIO_16_ACT_LED                     (0x01 << 18)    // GPIO pin 16 (ACT-LED) configured as output
#define     GPIO_PIN_16                         0x10
#define     GPIO_GPPUD_DISABLE_PULL_UP_DOWN     0x00            // 00 = Off – disable pull-up/down
#define     GPIO_GPPUDCLK0_PIN_14               (0x01 << 14)    // Assert clock on line
#define     GPIO_GPPUDCLK0_PIN_15               (0x01 << 14)    // Assert clock on line

#define     UART_CR_DISABLE_UART                0x00            // Disable the UART
#define     UART_ICR_CLEAR_INTERRUPT            0x7f3           // Clear the interrupts
#define     UART_IMSC_CLEAR_INTERRUPT_MASK      0x00            // Clear the interrupt mask set/clear register
#define     UART_IBRD                           0x01            // Integer baud rate divisor
#define     UART_FBRD                           0x28            // Fractional baud rate divisor
#define     UART_LCRH_ENABLE_8_BIT_DATA         (0x03 << 5)     // Word length 8 bits
#define     UART_LCRH_ENABLE_FIFO               (0x01 << 4)     // Transmit and receive FIFOs are enabled (FIFO mode)
#define     UART_IFLS_FIFO_LEVEL_SELECT         0x00            // Interrupt FIFO level select
#define     UART_IMSC_SET_RX                    0x10            // Receive interrupt mask
#define     UART_CR_UART_ENABLE                 0x01            // UART enable
#define     UART_CR_TX_ENABLE                   (0x01 << 8)     // Transmit enable
#define     UART_CR_RX_ENABLE                   (0x01 << 9)     // Receive enable


// Peripherals

// General Purpose I/O (BroadCom BMC2835 ARM Peripherals chapter 6 for details)
#define     GPIO_BASE                           0x20200000
#define     GPIO_OFFSET_GPFSEL0                 0x00            // GPIO Function Select
#define     GPIO_OFFSET_GPFSEL1                 0x04            // GPIO Function Select
#define     GPIO_OFFSET_GPFSEL2                 0x08            // GPIO Function Select
#define     GPIO_OFFSET_GPFSEL3                 0x0C            // GPIO Function Select
#define     GPIO_OFFSET_GPFSEL4                 0x10            // GPIO Function Select
#define     GPIO_OFFSET_GPFSEL5                 0x14            // GPIO Function Select
#define     GPIO_OFFSET_GPSET0                  0x1C            // GPIO Pin Output Set 0
#define     GPIO_OFFSET_GPSET1                  0x20            // GPIO Pin Output Set 1
#define     GPIO_OFFSET_GPCLR0                  0x28            // GPIO Pin Output Clear 0
#define     GPIO_OFFSET_GPCLR1                  0x2C            // GPIO Pin Output Clear 1
#define     GPIO_OFFSET_GPPUD                   0x94            // GPIO Pin Pull-up/down Enable
#define     GPIO_OFFSET_GPPUDCLK0               0x98            // GPIO Pin Pull-up/down Enable Clock 0
#define     GPIO_OFFSER_GPPUDCLK1               0x9C            // GPIO Pin Pull-up/down Enable Clock 1


// UART (BroadCom BMC2835 ARM Peripherals chapter 13 for details)
#define     UART_BASE                           0x20201000
#define     UART_OFFSET_DR                      0x000           // UART data register
#define     UART_OFFSET_RSRECR                  0x004           // UART receive status register/error clear register
#define     UART_OFFSET_FR                      0x018           // UART flag register
#define     UART_OFFSET_IBRD                    0x024           // UART integer Baud rate divisor
#define     UART_OFFSET_FBRD                    0x028           // UART fractional Baud rate divisor
#define     UART_OFFSET_LCRH                    0x02C           // UART line control register
#define     UART_OFFSET_CR                      0x030           // UART control register
#define     UART_OFFSET_IFLS                    0x034           // UART Interrupt FIFO level select reg
#define     UART_OFFSET_IMSC                    0x038           // UART interrupt mask clear/set register
#define     UART_OFFSET_RIS                     0x03C           // UART raw interrupt status register
#define     UART_OFFSET_MIS                     0x040           // UART masked interrupt status register
#define     UART_OFFSET_ICR                     0x044           // UART interrupt clear register
#define     UART_OFFSET_DMACR                   0x048           // UART DMA control register

#endif /* HARDWARE_SYSTEM_H_ */