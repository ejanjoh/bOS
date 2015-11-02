/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-28
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  main.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 6       Updated
 *      ver 8       Added support to start processes
 *      ver 11      Added configuration on the heap
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include "io.h"
#include "log.h"
#include "assert.h"
#include "process_control.h"

extern uint32_t _init_start;
extern uint32_t _init_end;
extern uint32_t _heap_start;
extern uint32_t _heap_end;
extern uint32_t _stack_start;
extern uint32_t _stack_end;
extern uint32_t _text_start;
extern uint32_t _text_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _rodata_start;
extern uint32_t _rodata_end;
extern uint32_t _data_start;
extern uint32_t _data_end;

extern void (* _puts)(const char *str, const uint32_t len);
extern int32_t _printf(const uint32_t len, const char *format, ...);
extern void init_dyn_mem(void *p, uint32_t size);
static void print_memory_layout(void);


void main(void)
{
    /*
     * add something...
     */
    
    puts("Standard out configured...\n", 100);
    puts("Standard in configured...\n\n", 100);
    print_memory_layout();

    /*  For Beaglebone Black (BBB):
     *  Public RAM (L1 RAM) is too small to contain the heap. To be in position to run
     *  the system with the heap the execution point have to be moved to L3 SDRAM. Until
     *  this is done we run without the heap.
     */
#ifdef BBB
    _printf(100, "NOTE: Dynamic memory allocation on the heap is NOT available...\n\n");
#else
    init_dyn_mem((void *) &_heap_start, ((uint32_t) &_heap_end) - ((uint32_t)&_heap_start));
#endif

    // Start all other processes - after this, this process, the idle process will have
    // the lowest priority and will only run if no one else would like too...
    proc_ctrl_init_proc();

    // from now this is the idle process always ready and never end...
    for (;;) ;
    ASSERT(0);
}


static void print_memory_layout(void)
{
    uint32_t start, end, size;

    _puts("\r\nbOS Kernel Memory Layout\r\nSections    Start       End         Size        words\n", 200);
    _puts("----------------------------------------------------------\n", 200);
    
    // .init section
    start = (uint32_t) &_init_start;
    end  = (uint32_t) &_init_end;
    size = end - start;
    _printf(200, ".init       %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // .heap section
    start = (uint32_t) &_heap_start;
    end  = (uint32_t) &_heap_end;
    size = end - start;
    _printf(200, ".heap       %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // -stack section
    start = (uint32_t) &_stack_start;
    end  = (uint32_t) &_stack_end;
    size = end - start;
    _printf(200, ".stack      %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // .text section
    start = (uint32_t) &_text_start;
    end  = (uint32_t) &_text_end;
    size = end - start;
    _printf(200, ".text       %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // .bss section
    start = (uint32_t) &_bss_start;
    end  = (uint32_t) &_bss_end;
    size = end - start;
    _printf(200, ".bss        %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // .rodata section
    start = (uint32_t) &_rodata_start;
    end  = (uint32_t) &_rodata_end;
    size = end - start;
    _printf(200, ".rodata     %p  %p  %p  %p\n", start, end, size, size >> 2);
    
    // .data section
    start = (uint32_t) &_data_start;
    end  = (uint32_t) &_data_end;
    size = end - start;
    _printf(200, ".data       %p  %p  %p  %p\n\n", start, end, size, size >> 2);
    
    return;
}



