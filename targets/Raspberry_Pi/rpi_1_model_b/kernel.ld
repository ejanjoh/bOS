/******************************************************************************
*
*       Autor:      Jan Johansson (ejanjoh)
*       Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
*       Created:    2013-06-13
*       Updated:    2015-05-05
*
*       bOS version history mapped on changes in this file:
*       ---------------------------------------------------
*       ver2:    Adjusted for bOS
*
******************************************************************************/

/*
 * Note: Some toolchains generate a initial underscore at all labels in the 
 * object files and it might cause that itís not possible to access the labels 
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
 
num_usr_proc = 0x10;                            /* number of processes in user mode */
num_sys_proc = 0x10;                            /* number of processes in system mode */

heap_size       = 1M;                           /* heap size */
usr_stack_size  = 0x1000 * num_usr_proc;        /* to be adjusted later when we know resp processes need(*)... */
sys_stack_size  = 0x1000 * num_sys_proc;        /* to be adjusted later when we know resp processes need(*)... */
exc_stack_size  = 0x400;                        /* stack size for respective exception modes */

/* (*) see PCB-list, e.g. used in SystemProcessStartup() for starting up respective process */

/* constitute the memory layout (memory map) for the system */
SECTIONS {

    /* @ address 0x0: interrupt vector table (ivt), size 8*4 byte (see start.s) */

    /* @ address 0x100:	ATAGs, size: ? (not used at the moment...) */

    /* @ addresses 0x7000 to 0x8000 are used as a temporary stack during the initiation of the system. */

    /* @ address 0x8000: start point of the system. The GPU controlling the start-up of the system and 
       start the CPU at this address. This can be changed, see comments in start.s */		
    . = 0x8000;
    _sections_start = .;

    /* section for basic setting-up of the system */
    _init_start = .;
    .init : {
        *(.init)
    }
    _init_end = .;

    /* section containing memory for dynamic allocation */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _heap_start = .;
    .heap : {
        __heap_start = .;
        . = . + heap_size;
        __heap_end = .;
    }
    _heap_end = .;

    /* section containing all stacks for user processes, system processes and all exception stacks */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _stack_start = .;
    .stack : {

        /* stack for processes in processor mode user */
        __stack_usr_bottom = .;
        . = . + usr_stack_size;
        __stack_usr_top = .;

        /* stack for processes in processor mode system */
        . = . + 4;                  /* add a slack... */
        __stack_sys_bottom = .;
        . = . + sys_stack_size;
        __stack_sys_top = .;

        /* stack for exceptions in processor mode supervisor */
        . = . + 4;                  /* add a slack... */
        __stack_exc_svc_bottom = .;
        . = . + exc_stack_size;
        __stack_exc_svc_top = .;

        /* stack for exceptions in processor mode abort */
        . = . + 4;                  /* add a slack... */
        __stack_exc_abt_bottom = .;
        . = . + exc_stack_size;
        __stack_exc_abt_top = .;

        /* stack for exceptions in processor mode undefined */
        . = . + 4;                  /* add a slack... */
        __stack_exc_und_bottom = .;
        . = . + exc_stack_size;
        __stack_exc_und_top = .;

        /* stack for exceptions in processor mode interrupt */
        . = . + 4;                  /* add a slack... */
        __stack_exc_irq_bottom = .;
        . = . + exc_stack_size;
        __stack_exc_irq_top = .;

        /* stack for exceptions in processor mode fast interrupt */
        . = . + 4;                  /* add a slack... */
        __stack_exc_fiq_bottom = .;
        . = . + exc_stack_size;
        __stack_exc_fiq_top = .;

    }
    _stack_end = .;

    /* section containing all code in user and system mode after the basic initialization.
       It also contains the code for exception handling. (and some "high level" initialization 
       not done in the .init section) */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _text_start = .;
    .text : {
        *(.text.startup)
        *(.text)
    }
    _text_end = .;

    /* (contains non-initialized data) */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _bss_start = .;
    .bss : {
        *(.bss)
        *(COMMON)
    }
    . = ALIGN(0x4);         /* because we need to zero out the section at start-up and it needs to be word aligned */
    _bss_end = .;

    /* (contains read only data) */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _rodata_start = .;
    .rodata : {
        *(.rodata)
        *(.rodata.str1.4)
    }
    _rodata_end = .;

    /* (contains initialized data) */
    . = . + 4;                  /* add a slack... */
    . = ALIGN(0x4);
    _data_start = .;
    .data : {
        *(.data)
    }
    _data_end = .;

    /* Collect in-sections not trapped above */
    . = . + 4;              /* add a slack... */
    . = ALIGN(0x4);
    /DISCARD/ : {
        *(*)
    }

    /* @ address 0x20000000 to 0x20ffffff: peripherals (see: BroadCom BMC2835 ARM Peripherals) */

    _sections_end = .;
}


