/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-07-20
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  semaphore.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 9       created
 *
 *
 *      Reference: 
 *
 ******************************************************************************/

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <stdint.h>
#include "process_control.h"

// The size of the wait list. As defined here the size is two number to large and
// this is as expected.
#define SEMAPHORE_WAIT_LIST_SIZE NUMB_PROC

// The init value of a mutex
#define MUTEX_INIT_VALUE 1

// Type for the wait list
typedef struct sem_proc {
    uint32_t pid;
    procPrio_t prio;
} sem_proc_t;

// Type for semaphores used in the system
typedef struct semaphore {
    uint32_t count;
    uint32_t waiting;
    sem_proc_t process_wait_list[SEMAPHORE_WAIT_LIST_SIZE];
} semaphore_t;


/* void semaphore_init(semaphore_t *pSemaphore, uint32_t initValue)
 *
 * pSemaphore:  Pointer to a semaphore.
 * initValue:   Init value of the semaphore
 *
 * out:         None
 *
 * description: Init the semaphore.
 *
 * depend:      None
 */
void semaphore_init(semaphore_t *pSemaphore, uint32_t initValue);


/* void semaphore_wait(semaphore_t *pSemaphore)
 *
 * pSemaphore:  Pointer to a semaphore.
 *
 * out:         None
 *
 * description: Performs a wait operation on a semaphore.
 *
 * depend:      None
 */
void semaphore_wait(semaphore_t *pSemaphore);


/* void semaphore_signal(semaphore_t *pSemaphore)
 *
 * pSemaphore:  Pointer to a semaphore.
 *
 * out:         None
 *
 * description: Performs a signal operation on a semaphore.
 *
 * depend:      None
 */
void semaphore_signal(semaphore_t *pSemaphore);


#endif /* SEMAPHORE_H_ */