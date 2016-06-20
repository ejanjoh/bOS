/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2016-06-15
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  message.h
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
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>


// type for message identification (do not use the first and last id in regular use)
typedef enum {msg_none_1 = 0, msg_ping, msg_none_2} message_id_t;

// type for a message used in for communication and synchronization between processes
typedef struct message { uint32_t receiver;
                         uint32_t sender;
                         message_id_t id;
                         void *data;
                         struct message *next_msg;          // internal handling
                       } message_t;


/* void msg_init(void)
 *
 * in:          None
 *
 * out:         None
 *
 * description: Initiate and setup structures needed for enable inter process
 *              communication using messages between processes.
 *
 * depend:      None
 *
 * note:        The function should be performed before we start any processes
 *              at all.
 */
void msg_init(void);


/* message_t *msg_create(uint32_t receiver, message_id_t id, void *data)
 *
 * in:          receiver    The process id (PID) of the process that should
 *                          receive the message.
 *
 *              id          This is a unique message identification identify
 *                          the ongoing process synchronization.
 *
 *              data        Pointer to potential data attached to the meassage.
 *                          If the message don't contain any data this pointer
 *                          must me NULL-terminated.
 *
 * out:         The receiver PID on success, zero on fail
 *
 * description: Create a message for synchronization between processes, using 
 *              messages.
 *
 * depend:      None
 */
message_t *msg_create(uint32_t receiver, message_id_t id, void *data);


/* uint32_t msg_send(message_t *msg)
 *
 * in:          msg         Pointer to the message to be sent.
 *
 * out:         The receiver PID on success, zero on fail
 *
 * description: Sends a message defined by the inparameter to the receiver.
 *
 * depend:      None
 *
 * note:        The function contain critical regions protecting the message 
 *              list.
 */
uint32_t msg_send(message_t *msg);


/* message_t *msg_receive(void)
 *
 * in:          None
 *
 * out:         Pointer to the received message, NULL if no messages was in list
 *
 * description: Get a unread message from the unread list. Note, this function 
 *              will remove the message from this list. If a process for some
 '              reason need to keep the message, it should be done local. 
 *
 * depend:      
 *
 * note:        The function contain critical regions protecting the message 
 *              list.
 */
message_t *msg_receive(void);


/* uint32_t msg_get_sender(message_t *msg)
 *
 * in:          Pointer to a message
 *
 * out:         PID of the process that sent the message
 *
 * description: Get the PID of the process that sent a message
 *
 * depend:      
 *
 * note:        
 */
uint32_t msg_get_sender(message_t *msg);


/* message_id_t msg_get_id(message_t *msg)
 *
 * in:          Pointer to a message
 *
 * out:         The message identity
 *
 * description: Get the message identity from a message.
 *
 * depend:      
 *
 * note:        
 */
message_id_t msg_get_id(message_t *msg);


/* void *msg_get_data(message_t *msg)
 *
 * in:          Pointer to a message
 *
 * out:         Pointer to attached data in a message
 *
 * description: Message can store a pointer to a data structure sent by 
 *              a message. This function returns a pointer to the data –
 *              if there is any data...
 *
 * depend:      
 *
 * note:        
 */
void *msg_get_data(message_t *msg);


/* uint32_t msg_delete(message_t *msg)
 *
 * in:          Pointer to a message
 *
 * out:         The reseiver PID of the messages
 *
 * description: Free the allocated dynamic memory keeping the message.
 *
 * depend:      
 *
 * note:        Delete the message ASAP after been used. If you have any function
 *              that for some reason store old messages or just ignore deleting it
 *              you have a memory leak and will soon or later run out of dynamic 
 *              memory...
 */
uint32_t msg_delete(message_t *msg);


/* void msg_sleep(void)
 *
 * in:          None
 *
 * out:         None
 *
 * description: Put your process to sleep if there is no messages in the in-list...
 *
 * depend:      
 *
 * note:        
 */
void msg_sleep(void);


#endif /* MESSAGE_H_ */