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


void print_memory_layout(void);


void main(void)
{
    print_memory_layout();

    /*
     * add something...
     */

    // start all other processes
    proc_ctrl_init_proc();

    // from now this is the idle process, allway´s ready and never end...
    for (;;) ;
    ASSERT(0);
}


void print_memory_layout(void)
{
    uint32_t start, end, size;

    puts("\r\nbOS Kernel Memory Layout\r\nSections    Start       End         Size        words\r\n", 200);
    puts("----------------------------------------------------------\r\n", 200);
    
    // .init section
    start = (uint32_t) &_init_start;
    end  = (uint32_t) &_init_end;
    size = end - start + 4;
    printf(200, ".init       %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // .heap section
    start = (uint32_t) &_heap_start;
    end  = (uint32_t) &_heap_end;
    size = end - start + 4;
    printf(200, ".heap       %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // -stack section
    start = (uint32_t) &_stack_start;
    end  = (uint32_t) &_stack_end;
    size = end - start + 4;
    printf(200, ".stack      %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // .text section
    start = (uint32_t) &_text_start;
    end  = (uint32_t) &_text_end;
    size = end - start + 4;
    printf(200, ".text       %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // .bss section
    start = (uint32_t) &_bss_start;
    end  = (uint32_t) &_bss_end;
    size = end - start + 4;
    printf(200, ".bss        %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // .rodata section
    start = (uint32_t) &_rodata_start;
    end  = (uint32_t) &_rodata_end;
    size = end - start + 4;
    printf(200, ".rodata     %p  %p  %p  %p\r\n", start, end, size, size >> 2);
    
    // .data section
    start = (uint32_t) &_data_start;
    end  = (uint32_t) &_data_end;
    size = end - start + 4;
    printf(200, ".data       %p  %p  %p  %p\r\n\r\n", start, end, size, size >> 2);
    
    return;
}



