/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-07 (ver 3)
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  hardware_system.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *      ver 10      Defines adjusted for a faster context switch
 *
 *
 *      Reference:
 *          1. ARM 1176JZF-S technical reference manual
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


// Metadata
#define     ARM32
#define     ARMv6
#define     RPI
#define     ARM_SYS32_MODE_ONLY
#define     PROCESSOR   ARM1176JZF-S
#define     BOARD       RASPBERRY_PI_MOD_B


// Access hardware registers, in C only...
#define GET32(addr) (*(volatile unsigned int *) (addr))
#define SET32(addr, val) (*((volatile unsigned int *) (addr)) = ((unsigned int) (val)))


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

#define     SPSR_CREATE_PCB                     0x140           // Used to create the PCB for every process (check up!)

// System addresses
#define     SYSTEM_START_ADDR                   0x00008000      // The system start address after GPU boot up
#define     INTERRUPT_VECTOR_TABLE_ADDR         0x00000000      // The address of the reset vector

#define     CPSR_SYSTEM_MODE_BM                 0x1F            // CPSR M[4:0]
#define     CPSR_IRQ_BM                         0x80            // Processor Status Register - IRQ bit
#define     CPSR_FIQ_BM                         0x40            // Processor Status Register - FIQ bit


// define _endings using registers
// _REG                 - the address to a hardware register
// _BASE                - the base address to a hardware register
// _OFFSET              - the offset to the base address in a hardware register

// define _endings that is not registers
// _MASK                - the ones defining the bit mask to manipulate hardware registers
// _PAT                 - a pattern or number to be written to a hardware register

#define     STACK_WATERMARK_PAT                     0xFF00FF00
#define     HEAP_WATERMARK_PAT                      0xFF00FF00
#define     AN150CYCLES_PAT                         150

// Peripherals

// General Purpose I/O (BroadCom BMC2835 ARM Peripherals chapter 6 for details)
#define     GPIO_BASE                               0x20200000
#define     GPIO_GPFSEL0_OFFSET                     0x00            // GPIO Function Select
#define     GPIO_GPFSEL1_OFFSET                     0x04            // GPIO Function Select
#define     GPIO_GPFSEL2_OFFSET                     0x08            // GPIO Function Select
#define     GPIO_GPFSEL3_OFFSET                     0x0C            // GPIO Function Select
#define     GPIO_GPFSEL4_OFFSET                     0x10            // GPIO Function Select
#define     GPIO_GPFSEL5_OFFSET                     0x14            // GPIO Function Select
#define     GPIO_GPSET0_OFFSET                      0x1C            // GPIO Pin Output Set 0
#define     GPIO_GPSET1_OFFSET                      0x20            // GPIO Pin Output Set 1
#define     GPIO_GPCLR0_OFFSET                      0x28            // GPIO Pin Output Clear 0
#define     GPIO_GPCLR1_OFFSET                      0x2C            // GPIO Pin Output Clear 1
#define     GPIO_GPPUD_OFFSET                       0x94            // GPIO Pin Pull-up/down Enable
#define     GPIO_GPPUDCLK0_OFFSET                   0x98            // GPIO Pin Pull-up/down Enable Clock 0
#define     GPIO_GPPUDCLK1_OFFSET                   0x9C            // GPIO Pin Pull-up/down Enable Clock 1

#define     GPIO_GPFSEL_GPIO_X0_TRIPLET_MASK        (0x7 << (3*0))
#define     GPIO_GPFSEL_GPIO_X1_TRIPLET_MASK        (0x7 << (3*1))
#define     GPIO_GPFSEL_GPIO_X2_TRIPLET_MASK        (0x7 << (3*2))
#define     GPIO_GPFSEL_GPIO_X3_TRIPLET_MASK        (0x7 << (3*3))
#define     GPIO_GPFSEL_GPIO_X4_TRIPLET_MASK        (0x7 << (3*4))
#define     GPIO_GPFSEL_GPIO_X5_TRIPLET_MASK        (0x7 << (3*5))
#define     GPIO_GPFSEL_GPIO_X6_TRIPLET_MASK        (0x7 << (3*6))
#define     GPIO_GPFSEL_GPIO_X7_TRIPLET_MASK        (0x7 << (3*7))
#define     GPIO_GPFSEL_GPIO_X8_TRIPLET_MASK        (0x7 << (3*8))
#define     GPIO_GPFSEL_GPIO_X9_TRIPLET_MASK        (0x7 << (3*9))

#define     GPIO_14_UART_TX_MASK                    (0x04 << 12)    // GPIO pin 14 (UART TX) config. as altern. funct. 0
#define     GPIO_15_UART_RX_MASK                    (0x04 << 15)    // GPIO pin 15 (UART RX) config. as altern. funct. 0
#define     GPIO_16_ACT_LED_MASK                    (0x01 << 18)    // GPIO pin 16 (ACT-LED) configured as output
#define     GPIO_PIN_16_MASK                        0x10
#define     GPIO_GPPUDCLK0_PIN_14_MASK              (0x01 << 14)    // Assert clock on line
#define     GPIO_GPPUDCLK0_PIN_15_MASK              (0x01 << 14)    // Assert clock on line

#define     GPIO_GPPUD_DISABLE_PULL_UP_DOWN_PAT     0x00            // 00 = Off – disable pull-up/down

// Interrupt Registers (BroadCom BMC2835 ARM Peripherals chapter 7 for details)
#define     INTR_BASE                               0x2000B000
#define     INTR_IRQ_BASIC_PEND_OFFSET              0x200
#define     INTR_IRQ_PEND_1_OFFSET                  0x204
#define     INTR_IRQ_PEND_2_OFFSET                  0x208
#define     INTR_FIQ_CTRL_OFFSET                    0x20C
#define     INTR_ENABLE_IRQ_1_OFFSET                0x210
#define     INTR_ENABLE_IRQ_2_OFFSET                0x214
#define     INTR_ENABLE_BASIC_IRQ_OFFSET            0x218
#define     INTR_DISABLE_IRQ_1_OFFSET               0x21C
#define     INTR_DISABLE_IRQ_2_OFFSET               0x220
#define     INTR_DISABLE_BASIC_IRQ_OFFSET           0x22C

#define     INTR_ENABLE_IRQ_1_TIMER_MASK            0x01
#define     INTR_UART_RX_PEND_MASK                  (1 << 25)

// System Timer (BroadCom BMC2835 ARM Peripherals chapter 12 for details)
/*
 * NOTE: Timer zero and two are used by the GPU, so avoid using them...
 *
 *      CO: is used by the GPU as timer tick (100 interrupts/second)
 *      C2: is used for lightweight timers by the GPU
 */
#define     SYSTEM_TIMER_BASE                       0x20003000
#define     SYSTEM_TIMER_CS_OFFSET                  0x00            // control/status register
#define     SYSTEM_TIMER_CLO_OFFSET                 0x04            // counter, lower 32 bits
#define     SYSTEM_TIMER_CHI_OFFSET                 0x08            // counter, higher 32 bits
#define     SYSTEM_TIMER_C0_OFFSET                  0x0C            // timer compare 0, see note
#define     SYSTEM_TIMER_C1_OFFSET                  0x10            // timer compare 1
#define     SYSTEM_TIMER_C2_OFFSET                  0x14            // timer compare 2, see note
#define     SYSTEM_TIMER_C3_OFFSET                  0x18            // timer compare 3

#define     SYS_TIMER_CS_M0_MASK                    0x1
#define     SYS_TIMER_CS_M1_MASK                    0x2
#define     SYS_TIMER_CS_M2_MASK                    0x4
#define     SYS_TIMER_CS_M3_MASK                    0x8

#define     SYS_TIMER_INIT_DELAY_PAT                1000000
#define     SYS_TIMER_INTR_INTERVAL_PAT             0x2700          // gives approximately 100 ticks per second
#define     SYS_TIMER_GPU_DISTURB_PAT               0xF00000
#define     SYS_TIMER_TICKS_PER_SEC                 1000000         // 1 MHz timer...

// UART (BroadCom BMC2835 ARM Peripherals chapter 13 for details)
#define     UART_BASE                           0x20201000
#define     UART_DR_OFFSET                      0x000           // UART data register
#define     UART_RSRECR_OFFSET                  0x004           // UART receive status register/error clear register
#define     UART_FR_OFFSET                      0x018           // UART flag register
#define     UART_IBRD_OFFSET                    0x024           // UART integer Baud rate divisor
#define     UART_FBRD_OFFSET                    0x028           // UART fractional Baud rate divisor
#define     UART_LCRH_OFFSET                    0x02C           // UART line control register
#define     UART_CR_OFFSET                      0x030           // UART control register
#define     UART_IFLS_OFFSET                    0x034           // UART Interrupt FIFO level select reg
#define     UART_IMSC_OFFSET                    0x038           // UART interrupt mask clear/set register
#define     UART_RIS_OFFSET                     0x03C           // UART raw interrupt status register
#define     UART_MIS_OFFSET                     0x040           // UART masked interrupt status register
#define     UART_ICR_OFFSET                     0x044           // UART interrupt clear register
#define     UART_DMACR_OFFSET                   0x048           // UART DMA control register

#define     UART_FR_RX_FIFO_EMPTY_MASK          (0x1 << 4)
#define     UART_FR_TRANSMIT_FIFO_FULL_MASK     (0x1 << 5)
#define     UART_LCRH_ENABLE_8_BIT_DATA_MASK    (0x03 << 5)     // Word length 8 bits
#define     UART_LCRH_ENABLE_FIFO_MASK          (0x01 << 4)     // Transmit and receive FIFOs are enabled (FIFO mode)
#define     UART_CR_TX_ENABLE_MASK              (0x01 << 8)     // Transmit enable
#define     UART_CR_RX_ENABLE_MASK              (0x01 << 9)     // Receive enable
#define     UART_IMSC_SET_RX_MASK               0x10            // Receive interrupt mask
#define     UART_CR_UART_ENABLE_MASK            0x01            // UART enable

#define     UART_IMSC_CLEAR_INTERRUPT_MASK_PAT  0x00            // Clear the interrupt mask set/clear register
#define     UART_CR_DISABLE_UART_PAT            0x00            // Disable the UART
#define     UART_ICR_CLEAR_INTERRUPT_PAT        0x7f3           // Clear the interrupts
#define     UART_IBRD_PAT                       0x01            // Integer baud rate divisor
#define     UART_FBRD_PAT                       0x28            // Fractional baud rate divisor
#define     UART_IFLS_FIFO_LEVEL_SELECT_PAT     0x00            // Interrupt FIFO level select


#endif /* HARDWARE_SYSTEM_H_ */