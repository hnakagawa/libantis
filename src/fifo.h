#ifndef __FIFO_H_
#define __FIFO_H_

#include <stdlib.h>
#include <pthread.h>

#include "common.h"

#include "fifo_internal.h"

typedef struct _fifo fifo_t;

fifo_t* fifo_new(xmpp_ctx_t *ctx);

void fifo_delete(fifo_t *fifo);

int fifo_put(fifo_t *fifo, void *data);

int fifo_get(fifo_t *fifo, void *data);

int fifo_get_read_descriptor(fifo_t *fifo);

void fifo_set_nonblock(fifo_t *fifo);

void fifo_set_block(fifo_t *fifo);

int fifo_is_enable(fifo_t *self);

int fifo_enable(fifo_t *fifo);

int fifo_disable_writer(fifo_t *self);

int fifo_disable_reader(fifo_t *self);

#endif	/* __FIFO_H_ */
