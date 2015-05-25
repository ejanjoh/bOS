/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    2015-05-20
 *
 *      Project:    bOS/bOS/Beaglebone Black (rev A5C)
 *      File name:  misc_asm.S
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 3       File created
 *      ver 5       __aeabi_uidiv added
 *                  Copied to BBB
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
         * C prototype: uint32_t __aeabi_uidiv(uint32_t numerator,
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
        rem     .req r2             @ reminder, initially reminder = numerator
        iter    .req r3             @ iterate
        temp    .req r4             @ temporary

        @ if numerator is equal to zero
        cmp     rem, #0
        moveq   res, #0
        beq     100f

        @ if divisor is equal to zero
        cmp     div, #0
        ldreq   res, =0xffffffff
        beq     100f

        @ if numerator equal to divisor
        cmp     div, rem
        moveq   res, #1
        beq     100f

        @ if numerator is less then divisor
        cmp     rem, div
        movls   res, #0
        bls     100f
        
        mov     res, #0
        mov     iter, #0
        mov     temp, #0

        clz     iter, div           @ leading zeros on the divisor
        clz     temp, rem           @ leading zeros on the numerator
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





