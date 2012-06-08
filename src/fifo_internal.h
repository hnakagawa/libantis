#ifndef __FIFO_INTERNAL_H_
#define __FIFO_INTERNAL_H_

#include <stdlib.h>
#include <pthread.h>

struct fifo_internal
{
	int read_fd;
	int write_fd;
    size_t size;
};

struct fifo_internal* fifo_internal_build(struct fifo_internal *self, size_t size);

int fifo_internal_put(struct fifo_internal *self, void *data);

int fifo_internal_get(struct fifo_internal *self, void *data);

void fifo_internal_set_descriptor(struct fifo_internal *self, int write_fd, int read_fd);

int fifo_internal_get_read_descriptor(struct fifo_internal *self);

int fifo_internal_get_write_descriptor(struct fifo_internal *self);

void fifo_internal_set_nonblock(struct fifo_internal *self);

void fifo_internal_set_block(struct fifo_internal *self);

#endif	/* __FIFO_INTERNAL_H_ */
