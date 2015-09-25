/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-08-25
 *      Updated:    
 *
 *      Project:    Buffer
 *      File name:  buffer.h
 *
 *
 *      bOS version history mapped on changes in this file:
 *      ---------------------------------------------------
 *      ver 10      Created
 *
 *
 *      Reference:
 *
 ******************************************************************************/
 
#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>


typedef struct {
    uint32_t size;
    uint32_t cnt;
    char *buffer;
    char *read;
    char *write;
} fifo_t;


void fifo_init(fifo_t *fifo, char *buffer, const uint32_t size);
void fifo_write(fifo_t *fifo, const char c);
char fifo_read(fifo_t *fifo);

#endif /* BUFFER_H_ */