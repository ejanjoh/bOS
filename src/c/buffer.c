/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2015-08-25
 *      Updated:    
 *
 *      Project:    bOS
 *      File name:  buffer.c
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

#include <stddef.h>
#include "assert.h"
#include "buffer.h"


void fifo_init(fifo_t *fifo, char *buffer, uint32_t size)
{
    ASSERT(NULL != fifo);
    ASSERT(NULL != buffer);
    ASSERT(0 < size);

    fifo->size = size;
    fifo->cnt = 0;
    fifo->buffer = buffer;
    fifo->read = buffer;
    fifo->write = buffer;

    return;
}


void fifo_write(fifo_t *fifo, const char c)
{
    *fifo->write++ = c;
    fifo->cnt++;
    if (fifo->write == (fifo->buffer + fifo->size)) fifo->write = fifo->buffer;
    ASSERT(fifo->size >= fifo->cnt);
    return;
}


char fifo_read(fifo_t *fifo)
{
    char c = *fifo->read++;

    fifo->cnt--;
    if (fifo->read == (fifo->buffer + fifo->size)) fifo->read = fifo->buffer;
    ASSERT(fifo->size >= fifo->cnt);            // i.e. cnt < 0... (uint32_t)
    return c;
}


