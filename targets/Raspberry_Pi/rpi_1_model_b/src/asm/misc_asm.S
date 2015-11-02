/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    2015-09-08
 *
 *      Project:    bOS/Raspberry Pi (rev rpi_1_model_b)
 *      File name:  misc_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *      ver 5       __aeabi_uidiv added
 *      ver 10      get_system_timer_32 added
 *      ver 11      Added the modulo function __aeabi_uidivmod
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include "hardware_system.h"
 
         /*********************************************************************** 
         * memset32
         *
         * Set the first words of the block of memory pointed by pAddr to the
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
 

        /*********************************************************************** 
         * __aeabi_uidiv
         *
         * The ARM family of CPUs does not have a native integer division
         * instruction. So, division needs to be implemented by a library
         * function. The unsigned integer division is sorted out in the function
         * "__aeabi_uidiv" and this function are to be found in libgcc.a. 
         * However, when linking this project the linker can not find the
         * library nor the function, even if it is pointed out to the linker.
         * To handle this in a pragmatic way (I do not want to spend time just
         * now to find a more proper solution) this is an implementation that 
         * that will do the job...
         *
         * Perform a 32 bit unsigned integer division, using binary long
         * division. No error handling will be performed, however when divided
         * by zero the function will return 0xFFFFFFFF and it´s up to the user
         * to detect this.
         *
         * C prototype: uint32_t __aeabi_uidiv(uint32_t dividend,
         *                                     uint32_t divisor)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global __aeabi_uidiv
__aeabi_uidiv:
        stmfd   sp!, {r4, lr}
        mov     r2, r0

        res     .req r0             @ result of division
        div     .req r1             @ divisor
        rem     .req r2             @ reminder, initially reminder = dividend
        iter    .req r3             @ iterate
        temp    .req r4             @ temporary

        @ if dividend is equal to zero
        cmp     rem, #0
        moveq   res, #0
        beq     100f

        @ if divisor is equal to zero
        cmp     div, #0
        ldreq   res, =0xffffffff
        beq     100f

        @ if dividend equal to divisor
        cmp     div, rem
        moveq   res, #1
        beq     100f

        @ if dividend is less then divisor
        cmp     rem, div
        movls   res, #0
        bls     100f
        
        mov     res, #0
        mov     iter, #0
        mov     temp, #0

        clz     iter, div           @ leading zeros on the divisor
        clz     temp, rem           @ leading zeros on the dividend
        sub     iter, iter, temp
        lsl     div, div, iter
        add     iter, iter, #1

1:
        @ if reminder >= divisor then
        cmp     rem, div
        subhs   rem, rem, div
        addhs   res, res, #1

        @ are we done?
        sub     iter, iter, #1
        cmp     iter, #0
        beq     100f

        @ next...
        lsr     div, div, #1
        lsl     res, res, #1
        b       1b

100:
        .unreq  res
        .unreq  div
        .unreq  rem
        .unreq  iter
        .unreq  temp

        ldmfd   sp!, {r4, pc}


        /*********************************************************************** 
         * __aeabi_uidivmod
         *
         * The ARM family of CPUs does not have a native integer division
         * instruction nor modulo operation. So, modulo needs to be implemented
         * by a library function. The unsigned integer modulo operation is 
         * sorted out in the function "__aeabi_uidivmod" and this function are
         * to be found in libgcc.a.
         *
         * However, when linking this project the linker can not find the
         * library nor the function, even if it is pointed out to the linker.
         * To handle this in a pragmatic way (I do not want to spend time just
         * now to find a more proper solution) this is an implementation that 
         * that will do the job...
         *
         * Perform a 32 bit unsigned modulo operation, using binary long
         * division. No error handling will be performed, however when divided
         * by zero the function will return 0xFFFFFFFF and it´s up to the user
         * to detect this.
         *
         * (reminder = dividend mod divisor)
         *
         * C prototype: uint32_t __aeabi_uidivmod(uint32_t dividend,
         *                                        uint32_t divisor)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global __aeabi_uidivmod
__aeabi_uidivmod:
        stmfd   sp!, {r4, lr}
        mov     r2, r0

        res     .req r0             @ result of division
        div     .req r1             @ divisor
        rem     .req r2             @ reminder, initially reminder = dividend
        iter    .req r3             @ iterate
        temp    .req r4             @ temporary

        @ if dividend is equal to zero
        cmp     rem, #0
        moveq   res, #0
        beq     100f

        @ if divisor is equal to zero
        cmp     div, #0
        ldreq   res, =0xffffffff
        beq     100f

        @ if dividend equal to divisor
        cmp     div, rem
        moveq   res, #1
        beq     100f

        @ if dividend is less then divisor
        cmp     rem, div
        movls   res, #0
        bls     100f
        
        mov     res, #0
        mov     iter, #0
        mov     temp, #0

        clz     iter, div           @ leading zeros on the divisor
        clz     temp, rem           @ leading zeros on the dividend
        sub     iter, iter, temp
        lsl     div, div, iter
        add     iter, iter, #1

1:
        @ if reminder >= divisor then
        cmp     rem, div
        subhs   rem, rem, div
        addhs   res, res, #1

        @ are we done?
        sub     iter, iter, #1
        cmp     iter, #0
        beq     100f

        @ next...
        lsr     div, div, #1
        lsl     res, res, #1
        b       1b

100:
        mov r0, rem

        .unreq  res
        .unreq  div
        .unreq  rem
        .unreq  iter
        .unreq  temp

        ldmfd   sp!, {r4, pc}


        /*********************************************************************** 
         * get_system_timer_32
         *
         * Get the 32 bit value of the system timer.
         *
         * C prototype: uint32_t get_system_timer_32(void)
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global get_system_timer_32
get_system_timer_32:
        ldr     r1, =SYSTEM_TIMER_BASE
        ldr     r0, [r1, #SYSTEM_TIMER_CLO_OFFSET]
        mov     pc, lr



