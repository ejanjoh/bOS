/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-09
 *      Updated:    2016-05-18
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
 *      ver 11      Added the modulo function __aeabi_uidivmod
 *      ver 12      Added memcopy32, itoa32_hex
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
         * memcopy32
         *
         * Copies the values of the number of words from the location pointed by
         * the source directly to the memory block pointed by destination.
         *
         * C prototype: void memcopy32(void *pSource,
         *                             void *pDest,
         *                             uint32_t words)
         *
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global memcopy32
memcopy32:

        src     .req r0             /* pointer to the source */
        dest    .req r1             /* pointer to the destination */
        wrds    .req r2             /* number of words to be copied */

        stmfd   sp!, {r4-r11, lr}

8:
        cmp     wrds, #7
        bls     1f
        ldmia   src!, {r4-r11}
        stmia   dest!, {r4-r11}
        sub     wrds, wrds, #8
        b       8b

1:
        cmp     wrds, #0
        beq     0f
        ldmia   src!, {r4}
        stmia   dest!, {r4}
        sub     wrds, wrds, #1
        b       1b

0:
        ldmfd   sp!, {r4-r11, pc}
        .unreq src
        .unreq dest
        .unreq wrds


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
        mov     r0, rem

        .unreq  res
        .unreq  div
        .unreq  rem
        .unreq  iter
        .unreq  temp

        ldmfd   sp!, {r4, pc}

        /*********************************************************************** 
         * itoa32_hex
         *
         * Convert an integer value to a null-terminated string with the base
         * 16. The hexadicimal part is written as capital letters. A result
         * would be on the form 0x00FF00FF. All 32 bites are included even if 
         * they are zeros and all result consist of 11 chars (bytes) included 
         * the null-termination. No error correction exists.
         *
         * C prototype: void itoa32_hex(uint32_t val, char *str) 
         **********************************************************************/
        .section .text
        .code 32
        .align 2
        .global itoa32_hex
itoa32_hex:

        int     .req r0             /* integer value (int) */
        ptr     .req r1             /* pointer to string (ptr) */
        chr     .req r2             /* character (chr) */
        cnt     .req r3             /* counter (cnt) */

        mov     chr, #'0'
        strb    chr, [ptr]
        mov     chr, #'x'
        strb    chr, [ptr, #1]!
        mov     cnt, #0

1:
        mov     chr, int
        lsr     chr, chr, #28
        cmp     chr, #9
        bhi     3f

2:      /* digit in range [0, 9] */
        add     chr, chr, #48
        b       4f

3:      /* digit in range [A, F] */
        add     chr, chr, #('A' - 10)

4:
        strb    chr, [ptr, #1]!
        lsl     int, int, #4
        add     cnt, cnt, #1

        cmp     cnt, #8
        beq     10f
        b       1b

10:
        mov     chr, #0
        strb    chr, [ptr, #1]!

        .unreq  int
        .unreq  ptr
        .unreq  chr
        .unreq  cnt

        mov     pc, lr


