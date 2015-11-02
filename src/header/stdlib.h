/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2011-11-07 (DynamicMemory.h)
 *      Updated:    2015-10-19 (stdlib.h)
 *
 *      Project:    bOS
 *      File name:  stdlib.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 11      Updated
 *
 *
 *      Reference:  ...
 *
 *      Note:       This file does not contain all standard functions found in 
 *                  stdlib.h. As matter of fact, they are neither standard nor 
 *                  part of any lib. The file name is just used as a container 
 *                  for functionality normally found in this file. The function 
 *                  prototypes may not even be the same as expected nor does 
 *                  the functions follow any standard; see the code for the
 *                  functions respectively.
 *
 ******************************************************************************/

#include <stdint.h>
#include "io.h"

#ifndef STDLIB_H_
#define STDLIB_H_

extern void *malloc_no_std(const uint32_t size);
extern int32_t free_no_std(void **p);


/* void init_dyn_mem(void *p, uint32_t size)                // Should we keep this prototype in the header? 
 *                                                          // It will be initialized in the assembler code anyway...
 * in:          p:      Pointer to the heap start address
 *              size:   The size of the heap (in bytes)
 *
 * out:                 None
 *
 * description:         Initiate the the the blocks/buffers that build up the 
 *                      heap.
 *
 */
void init_dyn_mem(void *p, uint32_t size);


/* void *malloc(const uint32_t size)
 *
 * in:          size:   The block size in bytes, requested from the heap.
 *
 * out:                 Pointer to the allocated block of memory allocated.
 *
 * description:         Allocate a block of dynamic memory from the heap, a
 *                      coherent memory block of size "size". On success the 
 *                      function return a (void) pointer the the memory block,
 *                      and on failure it returns a NULL pointer. If a zero 
 *                      sized block is requested, the function return a NULL
 *                      pointer and this must not be dereferenced. If there is
 *                      available space in the heap a block of at least size 
 *                      "size" is returned.
 *
 */
#ifdef malloc
#undef malloc
#endif /* malloc */

#ifdef HEAP_DBG
#define malloc(size)                                                                                                \
    malloc_no_std((size));                                                                                          \
    do {                                                                                                            \
        printf(100, "HEAP_DBG: %u bytes block requested by malloc at %s, line %u\n", (size), __FILE__, __LINE__);   \
    } while(0)
#else
#define malloc(size) malloc_no_std((size))
#endif /* HEAP_DBG */


/* uint32_t free(void *p)
 *
 * in:          p:      Pointer to the block of memory to be deallocated.
 *
 * out:                 The size of the released block.
 *
 * description:         Deallocate a block of dynamic memory allocated from the
 *                      heap. On success the function return the size of the 
 *                      block and on failure it returns zero.
 */
#ifdef free
#undef free
#endif /* free */

#ifdef HEAP_DBG
#define free(p)                                                                                                 \
    free_no_std(&(p));                                                                                          \
    do {                                                                                                        \
        printf(100, "HEAP_DBG: Memory block pointed out by %p released. (%s, %u)\n", (p), __FILE__, __LINE__);  \
    } while (0)
#else
#define free(p) free_no_std(&(p))
#endif /* HEAP_DBG */


/* uint32_t atoi(char *str)
 *
 * in:          str:    Character string describing the integer to be
 *                      converted. Input can be on decimal or hexadecimal form.
 *
 * out:                 The converted integer
 *
 * description:         Convert a string describing an integer to an integer.
 *                      On success the function return a 32 bit integer and on
 *                      failure it returns zero. In this case it it up to the 
 *                      user to determine if the input is correct or not.
 */
uint32_t atoi(char *str);

#endif /* STDLIB_H_ */
