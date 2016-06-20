/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2011-11-07 (DynamicMemory.c)
 *      Updated:    2016-05-19 (dyn_mem.c)
 *
 *      Project:    bOS
 *      File name:  dyn_mem.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 11      Updated
 *      ver 12      Cosmetic change
 *      ver 13      Changed a modulo operation to increase performance and
 *                  added some extra 8 word sized buffers needed by the message 
 *                  handling:
 *
 *
 *      Reference:
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "stdlib.h"
#include "assert.h"
#include "semaphore.h"

int32_t _printf(const uint32_t len, const char *format, ...);

// Type for nodes in the heap table
typedef struct {
    uint32_t    nodeNo;             // node number in the heap table
    uint32_t    bufferSize;         // the buffer size available at node (in bytes)
    uint32_t    noFree;             // the number of free buffer
    uint32_t    noTot;              // the total available buffers with specified size above
    uint32_t    noMaxUsed;          // the maximum number of used buffers
    uint32_t    noReq;              // the total number of requests for this size
    uint32_t *  pFirstBuffer;       // pointer to the first available buffer
    uint32_t *  pLastBuffer;        // pointer to the last available buffer
    uint32_t *  pBufferLowAddr;     // map table: node <--> lowest memory address of current buffer size 
    uint32_t *  pBufferHighAddr;    // map table: node <--> highest memory address of current buffer size
} heapnode_t;

// NOTE! Adjust and customize the heap table to the system need...
// Heap table, i.e. container for dynamic memory available buffers
static heapnode_t hTable[] = \
{{   0,      0,   1,   1,   0,   0,   NULL,   NULL,   NULL,   NULL},  // 0 words - the zero buffer
 {   1,      4,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},  // 1 words
 {   2,      8,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},  // 2 words
 {   3,     16,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},  // 4 words
 {   4,     32, 100, 100,   0,   0,   NULL,   NULL,   NULL,   NULL},  // etc
 {   5,     64,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {   6,    128,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {   7,    256,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {   8,    512,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {   9,   1024,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {  10,   2048,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {  11,   4096,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {  12,   8192,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {  13,  16384,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},
 {  14,  32768,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL},    // 8192 words
 {  15,  65536,   5,   5,   0,   0,   NULL,   NULL,   NULL,   NULL}};   // 16384 words
 
 

#define LOW_NODE 0                                          // the lowest node
//#define HIGH_NODE (sizeof(hTable)/sizeof(heapnode_t) - 1)   // the highest node <== check it up...
#define HIGH_NODE 15                                        // the highest node (C99 may calculate sizeof at runtime...)
#define MIN_BUFFER_SIZE (hTable + LOW_NODE)->bufferSize
#define MAX_BUFFER_SIZE (hTable + HIGH_NODE)->bufferSize

// Mutex to protect the heap table (hTable)
semaphore_t gMutexHeapTable;


// Static function declarations
static uint32_t calc_heap_size(void);
static uint32_t getnode_size(const uint32_t size);
static uint32_t getnode_ptr(void *p);
static uint32_t isbufferfree(const void *p, const uint32_t node);
static void *getbuffer(uint32_t node);
static uint32_t freebuffer(uint32_t node, void *p);
static uint32_t modulo_2n(uint32_t a, uint32_t n);


void init_dyn_mem(void *p, uint32_t size)
{
    uint32_t *pNodeStart = (uint32_t *) p;
    uint32_t node, buffer;
    uint32_t *pCurr, *pNext;
    
    
    ASSERT(HIGH_NODE == (sizeof(hTable)/sizeof(heapnode_t) - 1));
    ASSERT(p != NULL);                              // to be changed!!! check the boundary of the physical heap instead!
    ASSERT(size >= calc_heap_size());
    
    semaphore_init(&gMutexHeapTable, MUTEX_INIT_VALUE);

    // Set, for every node, pointers to the first and last buffers at every node (!= NULL)
    for (node = (LOW_NODE + 1); node <= HIGH_NODE; node++) {
        (hTable + node)->pFirstBuffer = 
        (hTable + node)->pBufferLowAddr = 
        pNodeStart;

        (hTable + node)->pLastBuffer = 
        (hTable + node)->pBufferHighAddr = 
        pNodeStart + (((hTable + node)->bufferSize * ((hTable + node)->noTot - 1)) >> 2);
    
        pNodeStart = pNodeStart + (((hTable + node)->bufferSize * (hTable + node)->noTot) >> 2);
    }
    
    // Insert a pointer to the next buffer in every buffer in respective node. The last
    // buffer in every node should not point the the first buffer in next node...
    for (node = (LOW_NODE + 1); node <= HIGH_NODE; node++) {
        pCurr = (hTable + node)->pFirstBuffer;
        pNext = pCurr + (((hTable + node)->bufferSize) >> 2);
        
        for (buffer = 0; buffer < ((hTable + node)->noTot - 1); buffer++) {
            *pCurr = (uint32_t) pNext;
            pCurr = pNext;
            pNext = pCurr + (((hTable + node)->bufferSize) >> 2);
        }
    }

    _printf(200, "Dynamic memory on the heap configured.\n   start at: %p\n   size used: %x\n   size allocated: %x\n\n", 
            p, calc_heap_size(), size);
    return;
}


void *malloc_no_std(const uint32_t size)
{
    void *ptr = NULL;

    if (0 == size)
        printf(100, "Warning: zero sized buffer requested from malloc, NULL pointer returned...\n"); // B_PRINT
    else if ((hTable + HIGH_NODE)->bufferSize < size)
        printf(100, "Warning: Too large buffer requested from malloc, NULL pointer returned...\n");  // B_PRINT
    else
        ptr = getbuffer(getnode_size(size));

    return ptr;
}


int32_t free_no_std(void **p)
{
    uint32_t node = LOW_NODE;
    uint32_t size = 0;
    
    if (NULL == *p) {
        printf(100, "Warning: pointer returning NULL to free\n");
    }
    else {
        node = getnode_ptr(*p);
        if (0 == isbufferfree(*p, node)) {
            size = freebuffer(node, *p);
            *p = NULL;
        }
    }

    return size;
}


static uint32_t calc_heap_size(void)
{
    uint32_t heapSize = 0;
    uint32_t node = LOW_NODE;

    for (node = LOW_NODE; node <= HIGH_NODE; node++) {
            heapSize += (hTable + node)->bufferSize*(hTable + node)->noTot;
    }

    return heapSize;
}


static uint32_t getnode_size(const uint32_t size)
{
    uint32_t node = LOW_NODE;

    ASSERT(MIN_BUFFER_SIZE != size);
    ASSERT(MAX_BUFFER_SIZE >= size);

    for (node = LOW_NODE + 1; node <= HIGH_NODE; node++) {
        if ((hTable + node)->bufferSize < size) continue;
        else return node;
    }

    return LOW_NODE;
}


static uint32_t getnode_ptr(void *p)
{
    uint32_t node, check;

    ASSERT((hTable + LOW_NODE + 1)->pBufferLowAddr <= (uint32_t *) p && 
           (uint32_t *) p <= (hTable + HIGH_NODE)->pBufferHighAddr);

    // Find the correct node
    for (node = LOW_NODE + 1; node <= HIGH_NODE; node++) {
        if ((hTable + node)->pBufferLowAddr <= (uint32_t *) p && 
            (uint32_t *) p <= (hTable + node)->pBufferHighAddr) {

            // check that it's a correct buffer returned
            check = (uint32_t) p - (uint32_t) (hTable + node)->pBufferLowAddr;
            //if ( (check % ((hTable + node)->bufferSize ))){
            if ( modulo_2n(check, (hTable + node)->bufferSize)  ) {
                printf(100, "Warning: (1) pointer is not a correct heap buffer pointer, and can not be returned...\n");
                ASSERT(0);
                return LOW_NODE;
            } // if (check...)

            return node;
        } // if ((hTable...)
    }  // for (node...)

    printf(100, "Warning: (2) pointer is not a correct heap buffer pointer, and can not be returned...\n");
    ASSERT(0);
    return LOW_NODE;
}


// return zero if allocated else one
static uint32_t isbufferfree(const void *p, const uint32_t node)
{
    uint32_t *ptr = (hTable + node)->pFirstBuffer;

    while (1) {
        if ( ((uint32_t *) p) == ptr ) {
            break;
        }
        else if ((hTable + node)->pLastBuffer == ptr) {
            ptr = NULL;
            break;
        } 
        else {
            ptr = (uint32_t *) *ptr;
        }
    }

    return (uint32_t) (NULL != ptr);
}


static void *getbuffer(uint32_t node)
{
    uint32_t *pBuffer = NULL;

    ASSERT(LOW_NODE != node);
    ASSERT(HIGH_NODE >= node);

    /* *** CRITICAL REGION - START *** */
    semaphore_wait(&gMutexHeapTable);
    (hTable + node)->noReq++;
    
    while (1) {
        if ((hTable + node)->noFree > 0) {
            (hTable + node)->noFree--;
            
            if (((hTable + node)->noTot - (hTable + node)->noFree) > (hTable + node)->noMaxUsed)
                (hTable + node)->noMaxUsed++;
            
            pBuffer = (hTable + node)->pFirstBuffer;
            
            if ((hTable + node)->noFree == 0)
                (hTable + node)->pFirstBuffer = (hTable + node)->pLastBuffer = NULL;
            else
                (hTable + node)->pFirstBuffer = (uint32_t *) *((hTable + node)->pFirstBuffer);
                
            break;
        }
        else {
            printf(100, "Warning: no available buffer at node (size): %u (%u)\n", node, (hTable + node)->bufferSize);//B_PRINT
            node++;
            ASSERT(HIGH_NODE >= node);
            if (HIGH_NODE < node) break;  // if NDEBUG is not set and we don't have the protection from the ASSERT
        }
    }
    semaphore_signal(&gMutexHeapTable);
    /* *** CRITICAL REGION - END *** */
    
    // Add a warning and notification if we get short of free buffers...

    ASSERT(NULL != pBuffer);
    return (void *) pBuffer;
}


static uint32_t freebuffer(uint32_t node, void *p)
{
    if (LOW_NODE != node) {
        /* *** CRITICAL REGION - START *** */
        semaphore_wait(&gMutexHeapTable);
        if ((hTable + node)->noFree > 0) {
            *((void **)((hTable + node)->pLastBuffer)) = p;
            (hTable + node)->pLastBuffer = p;
            (hTable + node)->noFree++;
        }
        else {
            (hTable + node)->pFirstBuffer = (hTable + node)->pLastBuffer = p;
            (hTable + node)->noFree++;
        }
        semaphore_signal(&gMutexHeapTable);
        /* *** CRITICAL REGION - END *** */
    }
    
    return (hTable + node)->bufferSize;
}


// cmd - get_heap_node_info <size>
void _heap_node_info(uint32_t size)
{
    uint32_t node = LOW_NODE;

    if (0 == size)
        node = LOW_NODE;
    else if ((hTable + HIGH_NODE)->bufferSize < size)
        node = HIGH_NODE + 1;
    else
        node = getnode_size(size);
    
    if (node <= HIGH_NODE) {
        printf(100, "nodeNo:         %u\n", (hTable + node)->nodeNo);
        printf(100, "buffSize:       %u\n", (hTable + node)->bufferSize);
        printf(100, "noFree:         %u\n", (hTable + node)->noFree);
        printf(100, "noTot:          %u\n", (hTable + node)->noTot);
        printf(100, "noMaxUsed:      %u\n", (hTable + node)->noMaxUsed);
        printf(100, "noReq:          %u\n", (hTable + node)->noReq);

        printf(100, "pFirstBuff:     %p\n", (void *) (hTable + node)->pFirstBuffer);
        printf(100, "pLastBuff:      %p\n", (void *) (hTable + node)->pLastBuffer);
        printf(100, "pBuffLowAddr:   %p\n", (void *) (hTable + node)->pBufferLowAddr);
        printf(100, "pBuffHighAddr:  %p\n", (void *) (hTable + node)->pBufferHighAddr);
    }
    else {
        printf(100, "No buffers exist matching this size\n");
    }

    return;
}

// if the divisor is on the form 2^n the modulo function become very easy...
static uint32_t modulo_2n(uint32_t a, uint32_t n)
{
    return (a & (n - 1));
}


