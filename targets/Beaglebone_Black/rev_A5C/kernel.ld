/******************************************************************************
*
*   Created by:     Jan Johansson (ejanjoh)
*   Copyright:      Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
*   Date:           2013-10-11
*   Updated:        2016-05-12
*
*       Project:    bOS/Beaglebone Black (rev A5C)
*       File name:  kernel.ld
*
*
*   bOS version history mapped on changes in this file:
*   ---------------------------------------------------
*   ver 10          Increased the sys stack and number of processes running
*   ver 12          Updated to move the execution to level 3 SDRAM
*
******************************************************************************/

/*
 * Note: Some toolchains generate a initial underscore at all labels in the
 * object files and it might cause that it's not possible to access the labels
 * in the linker script if not adjusted...
 *
 * However, in this script the following naming convention are used for labels:
 * - no underscore for local labels
 * - one underscore for labels in the script on an out-sections level
 * - two underscores for labels in the script on an input-section level
 * - if not above, e.g. adjusted for some lib or so, it's commented...
 *   (all labels will be visible in the out section, use it with care (no local
 *    will exist in reality using this toolchain...)
 */



/*             |                    |
 *             |                    |
 *     ________|____________________| 0x9FFF_FFFF
 *             | EMIF0 SDRAM        |
 *             | (only 512 MB used  |
 *             | of 1 GB)           |
 *             |                    |
 *  L3 SDRAM   |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *     ________|____________________| 0x8000_0000
 *             | Reserved &         |
 *             | Peripherals        |
 *             |                    |
 *             |                    |
 *     ------- |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *     ________|____________________| 0x4030_FFFF
 *             | (Static Variables) |
 *             |____________________|
 *             | (Tracing Data)     |
 *             |____________________|
 *             | RAM Exc. Vectors   |
 *             |____________________| 0x4030_CE00
 *  Public RAM | (6KB Public stack) |
 *             |____________________| 0x4030_B800
 *             | Downloaded Image*  |
 *             |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *     ________|____________________| 0x402F_0400 (GP)
 *             |                    |
 *             |                    |
 *             |                    |
 *             |                    |
 *     ------- |~ ~ ~ ~ ~ ~ ~ ~ ~ ~ |
 *             |                    |
 *             |                    |
 *             |                    |
 *     ________|____________________| 0x0002_BFFF
 *             | ROM Version        |
 *             |____________________| 0x0002_BFFC
 *             | ROM Code           |
 *             |                    |
 *             |                    |
 *             |____________________| 0x0002_0100
 *         ROM | Dead Loops         |
 *             |____________________| 0x0002_0080
 *             | Public ROM CRC     |
 *             |____________________| 0x0002_0020
 *             | ROM Exc. Vector    |
 *             | Table              |
 *     ________|____________________| 0x0002_0000
 *             |                    |
 *             |                    |
 *
 *
 * (*) Downloaded Image
 * ====================  
 *   ::start     @ 0x402f0400
 *   .init
 *   (.heap)
 *   .stack
 *      - mode_sys: stack_sys_block_size * nbr_sys_proc
 *      - mode_und: stack_block_size
 *      - mode_abt: stack_block_size
 *      - mode_svc: stack_block_size
 *      - mode_irq: stack_block_size
 *      - mode_fiq: stack_block_size
 * _CPY_TO_L3_SDRAM_START
 *   .text
 *   .bss
 *   .rodata
 *   .data
 * _CPY_TO_L3_SDRAM_END
 *
 * Setup at level 3 SDRAM � Created during start of the system
 * ===========================================================
 *   .text      // copy from level 1 RAM
 *   .bss       // copy from level 1 RAM
 *   .rodata    // copy from level 1 RAM
 *   .data      // copy from level 1 RAM
 *   .heap      // created here (heap start, no copy of section...)
 *
 *
 */

heap_size               = 0x1000;                   /* heap size � minimal but faster heap � if needed */
stack_block_size        = 0x400;                    /* stack block size */
stack_sys_block_size    = stack_block_size*5;       /* size of sys stack size per proc */
nbr_sys_proc            = 16;                       /* the number of sys proc running on the system */

_L3_SDRAM_START         = 0x80000000;               /* Start address of DDR3 SDRAM (1 GB) */
_l3_heap_size           = 1M;                       /* heap size */



/* constitute the memory layout (memory map) for the system */
SECTIONS {

    /* @ address 0x402f0400: start point of the system. */
    . = 0x402f0400;
    _sections_start = .;

    /* section for basic setting-up of the system */
    _init_start = .;
    .init : {
        *(.init)
    }
    _init_end = .;

    /* section containing memory for dynamic allocation */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _heap_start = .;
    .heap : {
        __heap_start = .;
        . = . + heap_size;
        __heap_end = .;
    }
    _heap_end = .;

    /* section containing all stacks */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _stack_start = .;
    .stack : {
        __stack_sys_bottom = .;
        /*. = . + 16*5*stack_block_size;*/
        . = . + stack_sys_block_size*nbr_sys_proc;
        __stack_sys_top = .;

        __stack_und_bottom = .;
        . = . + stack_block_size;
        __stack_und_top = .;

        __stack_abt_bottom = .;
        . = . + stack_block_size;
        __stack_abt_top = .;

        __stack_svc_bottom = .;
        . = . + stack_block_size;
        __stack_svc_top = .;

        __stack_irq_bottom = .;
        . = . + stack_block_size;
        __stack_irq_top = .;

        __stack_fiq_bottom = .;
        . = . + stack_block_size;
        __stack_fiq_top = .;
    }
    _stack_end = .;

    /* section containing all code in user and system mode after the basic
       initialization. It also contains the code for exception handling. (and
       some "high level" initialization not done in the .init section) */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _CPY_TO_L3_SDRAM_START = .;
    _text_start = .;
    .text : {
        *(.text.startup)
        *(.text)
    }
    _text_end = .;

    /* (contains non-initialized data) */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _bss_start = .;
    .bss : {
        *(.bss)
        *(COMMON)
    }
    . = ALIGN(0x4);   /* because we need to zero out the .bss at start-up */
    _bss_end = .;

    /* (contains read only data) */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _rodata_start = .;
    .rodata : {
        *(.rodata)
        *(.rodata.str1.4)
    }
    _rodata_end = .;

    /* (contains initialized data) */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    _data_start = .;
    .data : {
        *(.data)
    }
    _data_end = .;
    
    _CPY_TO_L3_SDRAM_END = .;

    /* Collect in-sections not trapped above */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    /DISCARD/ : {
        *(*)
    }

    _sections_end = .;
}



