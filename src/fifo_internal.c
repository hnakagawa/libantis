#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>

#include "fifo_internal.h"

struct fifo_internal* fifo_internal_build(struct fifo_internal *self, size_t size)
{
    self->size = size;//sizeof(void*);
	return self;
}

void fifo_internal_set_descriptor(struct fifo_internal *self, int write_fd, int read_fd)
{
	assert(self);
	self->write_fd = write_fd;
	self->read_fd = read_fd;
}

int fifo_internal_put(struct fifo_internal *self, void *data)
{
	int ret = 0;

	assert(self);
	assert(data);

	ret = write(self->write_fd, data, self->size);
	if (ret > 0)
		fsync(self->write_fd);

	return ret;
}

int fifo_internal_get(struct fifo_internal *self, void *data)
{
	int ret = 0;

	assert(self);

	if ((ret = read(self->read_fd, data, self->size)) != self->size)
		return -1;

	return self->size;
}

int fifo_internal_get_read_descriptor(struct fifo_internal *self)
{
	return self->read_fd;
}

int fifo_internal_get_write_descriptor(struct fifo_internal *self)
{
	return self->write_fd;
}

void fifo_internal_set_nonblock(struct fifo_internal *self)
{
	int flags = fcntl(self->read_fd, F_GETFL, 0);
	fcntl(self->read_fd, F_SETFL, O_NONBLOCK | flags);
}

void fifo_internal_set_block(struct fifo_internal *self)
{
	int flags = fcntl(self->read_fd, F_GETFL, 0);
	fcntl(self->read_fd, F_SETFL, ~O_NONBLOCK & flags);
}
