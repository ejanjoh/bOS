/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  misc_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/
 
 
         /*********************************************************************** 
         * memset32
         *
         * Sets the first words of the block of memory pointed by pAddr to the
         * specified value.
         *
         * C prototype: void memset32(void *pAddr, 
         *                            const uint32_t value,
         *                            uint32_t words)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global memset32
memset32:
    stmfd   sp!, {r4-r9, lr}
    mov     r4, r1
    mov     r5, r1
    mov     r6, r1
    mov     r7, r1
    mov     r8, r1
    mov     r9, r1
            
6:
    cmp     r2, #5
    bls     1f
    stmia   r0!, {r4-r9}
    sub     r2, r2, #6
    b       6b

1:
    cmp     r2, #0
    beq     0f
    stmia   r0!, {r4}
    sub     r2, r2, #1
    b       1b

0:
    ldmfd   sp!, {r4-r9, pc}
 

 