/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-05-28
 *      Updated:    2016-05-19
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
 *      ver 12      Added the heap and dynamic memory allocation 
 *                  on Beaglebone Black
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
#ifdef BBB
    extern uint32_t _CPY_TO_L3_SDRAM_START;
    extern uint32_t _L3_SDRAM_START;
    extern uint32_t _l3_heap_size;
#endif

extern void (* _puts)(const char *str, const uint32_t len);
extern int32_t _printf(const uint32_t len, const char *format, ...);
extern void init_dyn_mem(void *p, uint32_t size);
static void print_memory_layout(void);
#ifdef BBB
    static void bbb_xenable_heap();
#endif

void main(void)
{
    /*
     * add something...
     */
    
    puts("Standard out configured...\n", 100);
    puts("Standard in configured...\n\n", 100);
    print_memory_layout();

    // Enable dynamic memory allocation on the heap
#ifdef BBB
    bbb_xenable_heap();
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


#ifdef BBB
    static void bbb_xenable_heap(void)
    {
        uint32_t start, end, size;

        _printf(100, "NOTE: Dynamic memory allocation on the L1 RAM heap is disabled;\n");
        _printf(100, "point of execution and the heap is moved to DDR3 SDRAM (L3).\n");
        _printf(100, "The following sections are copied to DDR3 SDRAM (including a\nnew heap):\n\n");

        _puts("\nbOS Kernel Memory Layout (on DDR3 SDRAM)\n", 200);
        _puts("Sections    Start       End         Size        words\n", 200);
        _puts("----------------------------------------------------------\n", 200);

        // .text section
        start = ((uint32_t) &_text_start - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        end = ((uint32_t) &_text_end - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        size = end - start;
        _printf(200, ".text       %p  %p  %p  %p\n", start, end, size, size >> 2);

        // .bss section
        start = ((uint32_t) &_bss_start - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        end = ((uint32_t) &_bss_end - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        size = end - start;
        _printf(200, ".bss        %p  %p  %p  %p\n", start, end, size, size >> 2);

        // .rodata section
        start = ((uint32_t) &_rodata_start - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        end = ((uint32_t) &_rodata_end - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        size = end - start;
        _printf(200, ".rodata     %p  %p  %p  %p\n", start, end, size, size >> 2);

        // .data section
        start = ((uint32_t) &_data_start - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        end = ((uint32_t) &_data_end - (uint32_t) &_CPY_TO_L3_SDRAM_START) - (uint32_t) &_L3_SDRAM_START;
        size = end - start;
        _printf(200, ".data       %p  %p  %p  %p\n", start, end, size, size >> 2);

        // .heap section
        start = end + 32;       // add a protection
        start = start >> 4;     // alignment
        start = start << 4;
        size = (uint32_t) &_l3_heap_size;
        end  = start + size;
        _printf(200, ".heap       %p  %p  %p  %p\n\n", start, end, size, size >> 2);

        // initiate and enable the heap
        init_dyn_mem((void *) start, size);

        return;
    }
#endif
