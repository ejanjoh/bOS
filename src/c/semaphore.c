/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-07-20
 *      Updated:    2015-09-08
 *
 *      Project:    bOS
 *      File name:  semaphore.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 9       created
 *      ver 10      Changed how the semaphore_signal behave; it should always
 *                  perform a context switch at the end of function. 
 *
 *
 *      Reference: 
 *
 ******************************************************************************/

#include <stdint.h>
#include "io.h"
#include "semaphore.h"
#include "process_control.h"


#define PROCESS_WAIT_LIST_INIT_VALUE (sem_proc_t) {0, prio_idle_proc}

static void add_process_to_wait_list(semaphore_t *pSemaphore);
static void remove_process_from_wating_list(semaphore_t *pSemaphore);


void semaphore_init(semaphore_t *pSemaphore, uint32_t initValue)
{    
    // set the start value of the semaphore
    pSemaphore->count = initValue;
    
    // the number of waiting processes
    pSemaphore->waiting = 0;

    // init the process wait list
    for (uint32_t i = 0; i < SEMAPHORE_WAIT_LIST_SIZE; i++)
        *(pSemaphore->process_wait_list + i) = PROCESS_WAIT_LIST_INIT_VALUE;

    return;
}


void semaphore_wait(semaphore_t *pSemaphore)
{
    DISABLE_INTERRUPT;

    if (0 < pSemaphore->count) {
        pSemaphore->count--;
    }
    else {
        add_process_to_wait_list(pSemaphore);
        proc_ctrl_context_switch(blocked_semaphore);
    }

    ENABLE_INTERRUPT;
    return;
}


void semaphore_signal(semaphore_t *pSemaphore)
{
    DISABLE_INTERRUPT;
    
    if (0 == pSemaphore->waiting) {
        pSemaphore->count++;
    }
    else {
        remove_process_from_wating_list(pSemaphore);
    }
    
    proc_ctrl_context_switch(ready);
    ENABLE_INTERRUPT;
    return;
}


static void add_process_to_wait_list(semaphore_t *pSemaphore)
{
    sem_proc_t swap, proc;
    uint32_t i = 0;

    proc.pid = proc_ctrl_get_curr_pid();
    proc.prio = proc_ctrl_get_curr_prio();


    while (1) {
        if ((*(pSemaphore->process_wait_list + i)).prio > proc.prio) {
            swap = *(pSemaphore->process_wait_list + i);
            *(pSemaphore->process_wait_list + i) = proc;
            proc = swap;
        }
    
        if (prio_idle_proc == proc.prio) break;
        i++;
    }

    pSemaphore->waiting++;
    return;
}


static void remove_process_from_wating_list(semaphore_t *pSemaphore)
{
    const uint32_t pid = (*(pSemaphore->process_wait_list)).pid;

    for (uint32_t i = 0; i < pSemaphore->waiting; i++)
        *(pSemaphore->process_wait_list + i) = *(pSemaphore->process_wait_list + i + 1);

    pSemaphore->waiting--;
    proc_ctrl_change_state(pid, ready);
    return;
}


// ******** Test only ***********


semaphore_t mutexTest;


void print_wait_list(semaphore_t *pSemaphore)
{
    for (uint32_t i = 0; i < SEMAPHORE_WAIT_LIST_SIZE; i++) {
        printf(100, "(%u, %u, %u, %u) ", 
               pSemaphore->process_wait_list[i].pid, pSemaphore->process_wait_list[i].prio, 
               pSemaphore->waiting, pSemaphore->count);
    }
    puts("\n", 100);

    return;
}



// ******** Test only ***********
/*
void test_mutex(void)
{
    static uint32_t notPass = 0;
    
    
    uint32_t pid = proc_ctrl_get_curr_pid();
    
    printf(100, "process %u has entered the mutex test function\n", pid);
    print_wait_list(&mutexTest);
    
    semaphore_wait(&mutexTest);
    printf(100, "process %u has entered the protected area\n", pid);
    print_wait_list(&mutexTest);
    
    if (!notPass++) proc_ctrl_context_switch(ready);
    
    semaphore_signal(&mutexTest);
    
    printf(100, "process %u has left the protected area\n", pid);
    print_wait_list(&mutexTest);

    return;
}


void procA(void)
{
    semaphore_init(&mutexTest, MUTEX_INIT_VALUE);
    test_mutex();
    while (1) ;
    return;
}


void procB(void)
{
    test_mutex();
    while (1) ;
    return;
}


void procC(void)
{
    test_mutex();
    while (1) ;
    return;
}


void procD(void)
{
    test_mutex();
    while (1) ;
    return;
}
*/




