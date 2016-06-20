/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2016-06-15
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  message.c
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 13      created
 *
 *
 *      Reference: See hardware_system.h
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "message.h"
#include "stdlib.h"
#include "assert.h"
#include "sys_def.h"
#include "process_control.h"
#include "semaphore.h"

extern void (* _puts)(const char *str, const uint32_t len);

// que type for keeping the messages to the processes
typedef struct  { uint32_t nbr_in_que;
                  message_t *next_msg;
                  message_t *last_msg;
                } msg_list_t;


// keeping the list of messages to the processes
static msg_list_t msg_list[NUMB_PROC];

// Mutex to protect the message list
semaphore_t g_mutex_msg_list;


void msg_init(void)
{
    // init the message list
    for (uint32_t i = 0; i < NUMB_PROC; i++) {
        msg_list[i].nbr_in_que = 0;
        msg_list[i].next_msg = NULL;
        msg_list[i].last_msg = NULL;
    }

    // init the semaphore protecting the message list
    semaphore_init(&g_mutex_msg_list, MUTEX_INIT_VALUE);
    _puts("IPC by messages configured...\n\n", 100);
    return;
}


message_t *msg_create(uint32_t receiver, message_id_t id, void *data)
{
    message_t *msg = NULL;

    ASSERT(PID_IDLE != receiver);
    ASSERT(HIGH_PID >= receiver);
    ASSERT(msg_none_1 < id && id < msg_none_2);
    //if (NULL != data) ASSERT(/* check that the data ptr pointing into the heap... */ 1);
    
    msg = (message_t *) malloc(sizeof(message_t));
    if (NULL != msg) {
        msg->sender = proc_ctrl_get_curr_pid();
        msg->receiver = receiver;
        msg->id = id;
        msg->data = data;
        msg->next_msg = NULL;
    }
    else {
        ASSERT(0);
    }

    return msg;
}


uint32_t msg_send(message_t *msg)
{
    uint32_t ret = 0;


    ASSERT(NULL != msg);
    // ASSERT if the msg not pointing on the heap

    ASSERT(PID_IDLE != msg->receiver);
    ASSERT(HIGH_PID >= msg->receiver);
    ASSERT(msg_none_1 < msg->id && msg->id < msg_none_2);
    ret = msg->receiver;

    // Critical region starts...
    semaphore_wait(&g_mutex_msg_list);
    if (0 == msg_list[msg->receiver].nbr_in_que) {
        msg_list[msg->receiver].next_msg = msg;
        msg_list[msg->receiver].last_msg = msg;
    }
    else {
        msg_list[msg->receiver].last_msg->next_msg = msg;
        msg_list[msg->receiver].last_msg = msg;
    }
    
    msg_list[msg->receiver].nbr_in_que += 1;
    msg->next_msg = NULL;

    // semaphore_signal will generate a context switch. Make sure that the
    // receiving process is in state ready.
    if (msg->receiver != proc_ctrl_get_curr_pid()) {
        proc_ctrl_change_state(msg->receiver, ready);
    }
    
    // Critical regions ends...
    semaphore_signal(&g_mutex_msg_list);
    
    msg = NULL;
    return ret;
}


message_t *msg_receive(void)
{
    message_t *msg = NULL;
    const uint32_t pid = proc_ctrl_get_curr_pid();


    if (0 < msg_list[pid].nbr_in_que) {
        // Critical region starts...
        semaphore_wait(&g_mutex_msg_list);
        
        msg = msg_list[pid].next_msg;
        msg_list[pid].next_msg = msg_list[pid].next_msg->next_msg;
        msg_list[pid].nbr_in_que -= 1;
        
        // Critical regions ends...
        semaphore_signal(&g_mutex_msg_list);    
    }
    
    return msg;
}


uint32_t msg_get_sender(message_t *msg)
{
    return msg->sender;
}


message_id_t msg_get_id(message_t *msg)
{
    return msg->id;
}


void *msg_get_data(message_t *msg)
{
    return msg->data;
}


uint32_t msg_delete(message_t *msg)
{
    const uint32_t receiver = msg->receiver;


    msg->sender = 0;
    msg->receiver = 0;
    msg->id = 0;
    msg->data = NULL;
    msg->next_msg = NULL;

    if (0 == free(msg)) {
        ASSERT(0);
    }

    msg = NULL;
    return receiver;
}


void msg_sleep(void)
{
    const uint32_t pid = proc_ctrl_get_curr_pid();


    if (0 == msg_list[pid].nbr_in_que) {
        proc_ctrl_context_switch(blocked_msg);
    }

    return;
}





