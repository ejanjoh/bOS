/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2014-02-10 (github.com/ejanjoh/BeagleBone-Black/source/asm/
 *                              emif_ddr3.s)
 *      Updated:    2016-05-17
 *
 *      Project:    bOS/Beaglebone Black (rev A5C)
 *      File name:  emif_ddr3.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 12      File copied from Beaglebone Black project and updated
 *                  to fit the need, i.e. get the level 3 SDRAM up and running.
 *
 *      Reference: 
 *          (1) TI Reference Manual AM335x Cortex A8 Microprocessors
 *          (2) Micron MT41K256M16 data scheet
 *          (3) u-boot-2013.04
 *          (4) SPRS717F Texas Instrument
 *
 *      Note: This file is a copy from an other project and none or very small
 *            adaptations has been done; i.e. the coding style differs 
 *            somewhat from the rest of the project – tbc later on...
 *           
 *******************************************************************************
 *
 * License:  This program is free software; you can redistribute it and/or
 *           modify it under the terms of the GNU General Public License as
 *           published by the Free Software Foundation; either version 2 of the 
 *           License, or (at your option) any later version.
 *
 *           This program is distributed in the hope that it will be useful,
 *           but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *           MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the 
 *           GNU General Public License for more details.
 *
 ******************************************************************************/
 
/* ***** Registers ***** */
/* *** Base registers *** */
    .equ    DDR3_PHY,   0x44E12000          /* DDR3 phy registers */
    .equ    EMIF4D,     0x4C000000          /* EMIF4D registers */
    .equ    CTRLMODE,   0x44E10000          /* CTRLMOD registers */

/* *** Offset registers *** */
    /* 32 bit, base: DDR3_PHY  */
    .equ    CMD0_REG_PHY_CTRL_SLAVE_RATIO_0,            0x01C
    .equ    CMD0_REG_PHY_DLL_LOCK_DIFF_0,               0x028
    .equ    CMD0_REG_PHY_INVERT_CLKOUT_0,               0x02C
    
    .equ    CMD1_REG_PHY_CTRL_SLAVE_RATIO_0,            0x050
    .equ    CMD1_REG_PHY_DLL_LOCK_DIFF_0,               0x05C
    .equ    CMD1_REG_PHY_INVERT_CLKOUT_0,               0x060

    .equ    CMD2_REG_PHY_CTRL_SLAVE_RATIO_0,            0x084
    .equ    CMD2_REG_PHY_DLL_LOCK_DIFF_0,               0x090
    .equ    CMD2_REG_PHY_INVERT_CLKOUT_0,               0x094
    
    .equ    DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_0,         0x0C8
    .equ    DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_0,         0x0DC
    .equ    DATA0_REG_PHY_WRLVL_INIT_RATIO_0,           0x0F0
    .equ    DATA0_REG_PHY_GATELVL_INIT_RATIO_0,         0x0FC
    .equ    DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_0,        0x108
    .equ    DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_0,        0x120
    .equ    DATA0_REG_PHY_USE_RANK0_DELAYS,             0x134
    .equ    DATA0_REG_PHY_DLL_LOCK_DIFF_0,              0x138
    
    .equ    DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_0,         0x16C
    .equ    DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_0,         0x180
    .equ    DATA1_REG_PHY_WRLVL_INIT_MODE_0,            0x194
    .equ    DATA1_REG_PHY_GATELVL_INIT_RATIO_0,         0x1A0
    .equ    DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_0,        0x1AC
    .equ    DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_0,        0x1C4
    .equ    DATA1_REG_PHY_USE_RANK0_DELAYS,             0x1D8
    .equ    DATA1_REG_PHY_DLL_LOCK_DIFF_0,              0x1DC
    
    /* 32 bit, base: EMIF4D  */
    .equ    SDRAM_CONFIG,                               0x008
    .equ    SDRAM_REF_CTRL,                             0x010
    .equ    SDRAM_REF_CTRL_SHDW,                        0x014
    .equ    SDRAM_TIM_1,                                0x018
    .equ    SDRAM_TIM_1_SHDW,                           0x01C
    .equ    SDRAM_TIM_2,                                0x020
    .equ    SDRAM_TIM_2_SHDW,                           0x024
    .equ    SDRAM_TIM_3,                                0x028
    .equ    SDRAM_TIM_3_SHDW,                           0x02C
    .equ    ZQ_CONFIG,                                  0x0C8
    .equ    DDR_PHY_CTRL_1,                             0x0E4
    .equ    DDR_PHY_CTRL_1_SHDW,                        0x0E8
    
    /* 32 bit, base: CTRLMOD  */
    .equ    CONTROL_EMIF_SDRAM_CONFIG,                  0x110
    .equ    VTP_CTRL,                                   0xE0C
    .equ    DDR_CKE_CTRL,                               0x131C
    .equ    DDR_CMD0_IOCTRL,                            0x1408  /* (10) */
    .equ    DDR_CMD1_IOCTRL,                            0x140C  /* (10) */
    .equ    DDR_CMD2_IOCTRL,                            0x1410  /* (10) */
    .equ    DDR_DATA0_IOCTRL,                           0x1444  /* (10) */
    .equ    DDR_DATA1_IOCTRL,                           0x1448  /* (10) */
    /* (10) It looks like the offset addresses in ref (1) is wrong, adding four
            bytes to the offset addresses make it work... */

    
/* Register values */
    .equ    CMD_REG_PHY_CTRL_SLAVE_RATIO_0_VAL,         0x80
    .equ    CMD_REG_PHY_DLL_LOCK_DIFF_0_VAL,            0x1
    .equ    CMD_REG_PHY_INVERT_CLKOUT_0_VAL,            0x0
    
    .equ    DATA_REG_PHY_RD_DQS_SLAVE_RATIO_0_VAL,      0x38
    .equ    DATA_REG_PHY_WR_DQS_SLAVE_RATIO_0_VAL,      0x44
    .equ    DATA_REG_PHY_WRLVL_INIT_RATIO_0_VAL,        0x0
    .equ    DATA_REG_PHY_GATELVL_INIT_RATIO_0_VAL,      0x0
    .equ    DATA_REG_PHY_FIFO_WE_SLAVE_RATIO_0_VAL,     0x94
    .equ    DATA_REG_PHY_WR_DATA_SLAVE_RATIO_0_VAL,     0x7D
    .equ    DATA_REG_PHY_USE_RANK0_DELAYS_VAL,          0x0
    .equ    DATA_REG_PHY_DLL_LOCK_DIFF_0_VAL,           0x0
    
    .equ    DDR_CMD_DATA_IOCTRL_VAL,                    0x18B
    
    .equ    DDR_PHY_CTRL_1_VAL,                         0x100007
    
    .equ    SDRAM_TIM_1_VAL,                            0x0AAAD4DB
    .equ    SDRAM_TIM_2_VAL,                            0x266B7FDA
    .equ    SDRAM_TIM_3_VAL,                            0x501F867F
    
    .equ    SDRAM_REF_CTRL_TIMER_VAL,                   0x2800
    .equ    ZQ_CONFIG_VAL,                              0x50074BE4
    .equ    SDRAM_CONFIG_VAL,                           0x61C05332
    .equ    SDRAM_REF_CTRL_VAL,                         0xC30


/* DDR3 SDRAM 512 M start address */
    .equ    DDR3_SDRAM_START_ADDR,  0x80000000

/* Stop at “DDR3_SDRAM_STOP_ADDR – 0x1”, i.e. after 512 M */
    .equ    DDR3_SDRAM_STOP_ADDR,   0xA0000000



        /*********************************************************************** 
         * Config_EMIF_DDR3_SDRAM
         *
         * Configure the EMIF DDR3 SDRAM on the BBB
         * For details see references.
         *
         * C prototype: void Config_EMIF_DDR3_SDRAM(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global Config_EMIF_DDR3_SDRAM
Config_EMIF_DDR3_SDRAM:
        stmfd   sp!, {fp, lr}
        
        bl      ConfigVTP
        bl      ConfigCmdCtrl
        bl      ConfigDDR3_Data
        bl      ConfigIO_Ctrl
        
        /* Clock enable control, select power-down and self-refresh op... */
        ldr     r0, =CTRLMODE
        ldr     r1, =DDR_CKE_CTRL
        add     r0, r0, r1
        mov     r1, #0x1
        str     r1, [r0]
        
        bl      ConfigDDR3_Phy
        bl      Set_SDRAM_Timings
        bl      ConfigSDRAM

        ldmfd   sp!, {fp, pc}


        /*********************************************************************** 
         * ConfigVTP
         *
         * VTP calibration adjusts the output impedance of the DDR pins for
         * process, for voltage, temperature, and process variation. For details
         * see reference (1).
         *
         * C prototype: static void ConfigVTP(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigVTP:
        base    .req r0             /* the base address */
        val     .req r1             /* the value */
        tmp     .req r2             /* a temporary value */
        
        ldr     base, =CTRLMODE
        
        /* Dynamic VTP compensation mode */
        ldr     val, [base, #VTP_CTRL]
        orr     val, val, #(0x1 << 6)
        str     val, [base, #VTP_CTRL]
        
        /* Clears flops, start￼clears flops, start */
        ldr     val, [base, #VTP_CTRL]
        and     val, val, #(~(0x1))
        str     val, [base, #VTP_CTRL]
        
        ldr     val, [base, #VTP_CTRL]
        orr     val, val, #(0x1)
        str     val, [base, #VTP_CTRL]
        
        /* Wait for training sequence to complete */
1:
        ldr     val, [base, #VTP_CTRL]
        and     val, val, #(0x1 << 5)
        cmp     val, #(0x1 << 5)
        bne     1b
        
100:        
        .unreq  base
        .unreq  val
        .unreq tmp

        mov     pc, lr


        /*********************************************************************** 
         * ConfigCmdCtrl
         *
         * Configure DDR CMD control registers (The ASIC control of DDR)
         * For details see reference (1).
         *
         * C prototype: static void ConfigCmdCtrl(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigCmdCtrl:
        /* (a) Ratio value for address/command launch timing in DDR PHY macro.
           (b) The max number of delay line taps variation allowed while 
               maintaining the master DLL lock.
           (c) Core clock is passed on to SDRAM */
        
        ldr     r0, =DDR3_PHY
        
        mov     r1, #CMD_REG_PHY_CTRL_SLAVE_RATIO_0_VAL
        str     r1, [r0, #CMD0_REG_PHY_CTRL_SLAVE_RATIO_0]          /* (a) */
        mov     r1, #CMD_REG_PHY_DLL_LOCK_DIFF_0_VAL
        str     r1, [r0, #CMD0_REG_PHY_DLL_LOCK_DIFF_0]             /* (b) */
        mov     r1, #CMD_REG_PHY_INVERT_CLKOUT_0_VAL
        str     r1, [r0, #CMD0_REG_PHY_INVERT_CLKOUT_0]             /* (c) */

        mov     r1, #CMD_REG_PHY_CTRL_SLAVE_RATIO_0_VAL
        str     r1, [r0, #CMD1_REG_PHY_CTRL_SLAVE_RATIO_0]          /* (a) */
        mov     r1, #CMD_REG_PHY_DLL_LOCK_DIFF_0_VAL
        str     r1, [r0, #CMD1_REG_PHY_DLL_LOCK_DIFF_0]             /* (b) */
        mov     r1, #CMD_REG_PHY_INVERT_CLKOUT_0_VAL
        str     r1, [r0, #CMD1_REG_PHY_INVERT_CLKOUT_0]             /* (c) */

        mov     r1, #CMD_REG_PHY_CTRL_SLAVE_RATIO_0_VAL
        str     r1, [r0, #CMD2_REG_PHY_CTRL_SLAVE_RATIO_0]          /* (a) */
        mov     r1, #CMD_REG_PHY_DLL_LOCK_DIFF_0_VAL
        str     r1, [r0, #CMD2_REG_PHY_DLL_LOCK_DIFF_0]             /* (b) */
        mov     r1, #CMD_REG_PHY_INVERT_CLKOUT_0_VAL
        str     r1, [r0, #CMD2_REG_PHY_INVERT_CLKOUT_0]             /* (c) */
        
        mov     pc, lr


        /*********************************************************************** 
         * ConfigDDR3_Data
         *
         * Configure the physical data pins. First the lower 8 of the 16 bites
         * then the higher of the 16 bits...
         * For details see reference (1)
         *
         * C prototype: static void ConfigDDR3_Data(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigDDR3_Data:
        /* (a) Ratio value for Read DQS slave DLL for CS0.
           (b) Ratio value for Write DQS slave DLL for CS0.
           (c) The user programmable init ratio selection mode for Write 
               Leveling FSM.
           (d) The user programmable init ratio used by DQS Gate Training FSM 
               when DATA0/1/_REG_PHY_GATELVL_INIT_MODE_0 register value set 
               to 1.
           (e) Ratio value for fifo we for CS0.
           (f) Ratio value for write data slave DLL for CS0.
           (g) Each Rank uses its own delay.
           (h) The max number of delay line taps variation allowed while 
               maintaining the master DLL lock */

        base    .req r0             /* the base address */
        val     .req r1             /* the value */

        ldr     base, =DDR3_PHY

        mov     val, #DATA_REG_PHY_RD_DQS_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_0]    /* (a) */
        mov     val, #DATA_REG_PHY_WR_DQS_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_0]    /* (b) */
        mov     val, #DATA_REG_PHY_WRLVL_INIT_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_WRLVL_INIT_RATIO_0]      /* (c) */
        mov     val, #DATA_REG_PHY_GATELVL_INIT_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_GATELVL_INIT_RATIO_0]    /* (d) */
        mov     val, #DATA_REG_PHY_FIFO_WE_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_0]   /* (e) */
        mov     val, #DATA_REG_PHY_WR_DATA_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_0]   /* (f) */
        mov     val, #DATA_REG_PHY_USE_RANK0_DELAYS_VAL
        str     val, [base, #DATA0_REG_PHY_USE_RANK0_DELAYS]        /* (g) */
        mov     val, #DATA_REG_PHY_DLL_LOCK_DIFF_0_VAL
        str     val, [base, #DATA0_REG_PHY_DLL_LOCK_DIFF_0]         /* (h) */

        mov     val, #DATA_REG_PHY_RD_DQS_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_0]    /* (a) */
        mov     val, #DATA_REG_PHY_WR_DQS_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_0]    /* (b) */
        mov     val, #DATA_REG_PHY_WRLVL_INIT_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_WRLVL_INIT_MODE_0]       /* (c) */
        mov     val, #DATA_REG_PHY_GATELVL_INIT_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_GATELVL_INIT_RATIO_0]    /* (d) */
        mov     val, #DATA_REG_PHY_FIFO_WE_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_0]   /* (e) */
        mov     val, #DATA_REG_PHY_WR_DATA_SLAVE_RATIO_0_VAL
        str     val, [base, #DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_0]   /* (f) */
        mov     val, #DATA_REG_PHY_USE_RANK0_DELAYS_VAL
        str     val, [base, #DATA1_REG_PHY_USE_RANK0_DELAYS]        /* (g) */
        mov     val, #DATA_REG_PHY_DLL_LOCK_DIFF_0_VAL
        str     val, [base, #DATA1_REG_PHY_DLL_LOCK_DIFF_0]         /* (h) */

        .unreq  base
        .unreq  val
        
        mov     pc, lr

 
        /*********************************************************************** 
         * ConfigIO_Ctrl
         *
         * Configure physical properities of the pins pull-up/down, impedance
         * etc. For details see reference (1) chapter 9.3.1.88 - 92 (details on
         * the different bits on the registers used).
         *
         * C prototype: static void ConfigIO_Ctrl(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigIO_Ctrl:
        base    .req r0             /* the base address */
        offset  .req r1             /* the offset to the base address */
        val     .req r2             /* the value */
        
        ldr     base, =CTRLMODE
        ldr     val, =DDR_CMD_DATA_IOCTRL_VAL

        ldr     offset, =DDR_CMD0_IOCTRL
        str     val, [base, offset]
        ldr     offset, =DDR_CMD1_IOCTRL
        str     val, [base, offset]
        ldr     offset, =DDR_CMD2_IOCTRL
        str     val, [base, offset]
        ldr     offset, =DDR_DATA0_IOCTRL
        str     val, [base, offset]
        ldr     offset, =DDR_DATA1_IOCTRL
        str     val, [base, offset]

        .unreq  base
        .unreq  offset
        .unreq  val

        mov     pc, lr


        /*********************************************************************** 
         * ConfigDDR3_Phy
         *
         * Defines the latency for read data from DDR SDRAM in number of DDR 
         * clock cycles. IO receivers only powered up during a read.
         * For details see reference (1) chapter 7.3.5.33 - 34.
         *
         * C prototype: static void ConfigDDR3_Phy(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigDDR3_Phy:
        base    .req r0             /* the base address */
        val     .req r1             /* the value */
        
        ldr     base, =EMIF4D
        
        /* - Defines the latency for read data from DDR SDRAM in number of 
             DDR clock cycles.
           - IO receivers only powered up during a read. */
        
        ldr     val, =DDR_PHY_CTRL_1_VAL
        str     val, [base, #DDR_PHY_CTRL_1]
        ldr     val, =SDRAM_CONFIG_VAL
        str     val, [base, #SDRAM_CONFIG]

        ldr     val, =DDR_PHY_CTRL_1_VAL
        str     val, [base, #DDR_PHY_CTRL_1_SHDW]
        ldr     val, =SDRAM_CONFIG_VAL
        str     val, [base, #SDRAM_CONFIG]
        
        .unreq  base
        .unreq  val

        mov     pc, lr


        /*********************************************************************** 
         * Set_SDRAM_Timings
         *
         * Set timing values needed by the SDRAM.
         * For details see reference (1) chapter 7.3.5.7 - 12 (details on
         * the different bits on the registers used).
         *
         * C prototype: static void Set_SDRAM_Timings(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
Set_SDRAM_Timings:
        base    .req r0             /* the base address */
        val     .req r1             /* the value */

        ldr     base, =EMIF4D        

        ldr     val, =SDRAM_TIM_1_VAL
        str     val, [base, #SDRAM_TIM_1]
        ldr     val, =SDRAM_TIM_1_VAL
        str     val, [base, #SDRAM_TIM_1_SHDW]
        ldr     val, =SDRAM_TIM_2_VAL
        str     val, [base, #SDRAM_TIM_2]
        ldr     val, =SDRAM_TIM_2_VAL
        str     val, [base, #SDRAM_TIM_2_SHDW]
        ldr     val, =SDRAM_TIM_3_VAL
        str     val, [base, #SDRAM_TIM_3]
        ldr     val, =SDRAM_TIM_3_VAL
        str     val, [base, #SDRAM_TIM_3_SHDW]

        .unreq  base
        .unreq  val

        mov     pc, lr


        /*********************************************************************** 
         * ConfigSDRAM
         *
         * ZQ callibration; the last step before handing over to the micro
         * controller...
         * For details see reference (1) chapter:
         *  - 7.3.5.5           (i)
         *  - 7.3.5.29          (ii)
         *  - 9.3.1.5           (iii)
         *  - 7.3.5.3           (iv)
         *  - 7.3.5.6           (v)
         *
         * C prototype: static void ConfigSDRAM(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
ConfigSDRAM:
        baseEMIF4D      .req r0     /* the base address, EMIF4D */
        baseCTRLMODE    .req r1     /* the base address, control mode */        
        val             .req r2     /* the value */

        ldr     baseEMIF4D, =EMIF4D
        ldr     baseCTRLMODE, =CTRLMODE

        ldr     val, =SDRAM_REF_CTRL_TIMER_VAL
        str     val, [baseEMIF4D, #SDRAM_REF_CTRL]                  /* (i) */
        ldr     val, =ZQ_CONFIG_VAL
        str     val, [baseEMIF4D, #ZQ_CONFIG]                       /* (ii) */
        ldr     val, =SDRAM_CONFIG_VAL
        str     val, [baseCTRLMODE, #CONTROL_EMIF_SDRAM_CONFIG]     /* (iii) */
        ldr     val, =SDRAM_CONFIG_VAL
        str     val, [baseEMIF4D, #SDRAM_CONFIG]                    /* (iv) */
        ldr     val, =SDRAM_REF_CTRL_VAL
        str     val, [baseEMIF4D, #SDRAM_REF_CTRL]                  /* (i) */
        ldr     val, =SDRAM_REF_CTRL_VAL
        str     val, [baseEMIF4D, #SDRAM_REF_CTRL_SHDW]             /* (v) */
        
        ldr     val, =SDRAM_REF_CTRL_VAL
        str     val, [baseEMIF4D, #SDRAM_REF_CTRL]                  /* (i) */
        ldr     val, =SDRAM_REF_CTRL_VAL
        str     val, [baseEMIF4D, #SDRAM_REF_CTRL_SHDW]             /* (v) */
        ldr     val, =SDRAM_CONFIG_VAL
        str     val, [baseEMIF4D, #SDRAM_CONFIG]                    /* (iv) */

        .unreq  baseEMIF4D
        .unreq  baseCTRLMODE
        .unreq  val

        mov     pc, lr


         /********************************************************************** 
         * TestSDRAM
         *
         * Performs a linear test, a write and read test, to test that the 
         * DDR3 SDRAM is configured in a "proper" way. The test write the 
         * address of the first byte in every word in the SDRAM. When the SDRAM
         * is completely filled the test reads every word and make sure it has 
         * the correct values.
         *
         * Note: The check is testing all 512 M SDRAM and it take some time...
         *       The test is not complete in the sense that it prove that no
         *       issues excists; it only indicate that this shouldn't be the 
         *       case.
         *
         * C prototype: void TestSDRAM(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global TestSDRAM
TestSDRAM:
        addr    .req r0     /* address within DDR3 SDRAM */
        end     .req r1     /* end address within DDR3 SDRAM */
        val     .req r2     /* value read from DDR3 SDRAM */
        
        stmfd   sp!, {fp, lr}

        ldr     r0, =TEXT_3_TEST_SDRAM_START
        mov     r1, #80
        bl      _uart_puts
        
        /* Write values to the DDR3 SDRAM – fill every words with the first 
           bytes start address */
        ldr     r0, =TEXT_4_TEST_SDRAM_WRITE
        mov     r1, #80
        bl      _uart_puts        

        ldr     addr, =DDR3_SDRAM_START_ADDR
        ldr     end, =DDR3_SDRAM_STOP_ADDR
1:
        /*str     addr, [addr], #4*/
        str     addr, [addr]
        add     addr, addr, #4
        cmp     addr, end
        bne     1b
        
        /* Read and check values from the DDR3 SDRAM */
        stmfd   sp!, {r0, r1, r2}
        ldr     r0, =TEXT_5_TEST_SDRAM_READ
        mov     r1, #80
        bl      _uart_puts       
        ldmfd   sp!, {r0, r1, r2}
        ldr     addr, =DDR3_SDRAM_START_ADDR

10:
        ldr     val, [addr]
        cmp     addr, val
        bne     ERROR_TEST_SDRAM

        add     addr, addr, #4
        cmp     addr, end
        bne     10b
100:
        ldr     r0, =TEXT_6_TEST_SDRAM_OK
        mov     r1, #80
        bl      _uart_puts
101:
        .unreq  addr
        .unreq  end
        .unreq  val
        
        ldmfd   sp!, {fp, pc}
        
        /* Error – the value read is unexpected… */
ERROR_TEST_SDRAM:
        stmfd   sp!, {r4}
        mov     r4, r0
        ldr     r0, =TEXT_1_TEST_SDRAM_ERROR
        mov     r1, #80
        bl      _uart_puts
        ldr     r0, =TEXT_2_TEST_SDRAM_ERROR
        mov     r1, #80
        bl      _uart_puts

/* 
        mov     r0, r4
        mov     r1, #1
        bl      HexDump
*/
        ldmfd   sp!, {r4}
        b       101b





        /*********************************************************************** 
         * .section .rodata
         *
         **********************************************************************/
        .section .rodata
        .align 2
TEXT_1_TEST_SDRAM_ERROR:
        .asciz "Error - the value read is unexpected...\n\r"
        .align 2
TEXT_2_TEST_SDRAM_ERROR:
        .asciz "Printing the last address and word:\n\r"
        .align 2
TEXT_3_TEST_SDRAM_START:
        .asciz "Start a linear test of the DDR3 SDRAM, testing 512 M:\n\r"
        .align 2
TEXT_4_TEST_SDRAM_WRITE:
        .asciz "   - write 512 M\n\r"
        .align 2
TEXT_5_TEST_SDRAM_READ:
        .asciz "   - read 512 M\n\r"
        .align 2
TEXT_6_TEST_SDRAM_OK:
        .asciz "DDR3 SDRAM test completed – test passed...\n\r"
        .align 2
