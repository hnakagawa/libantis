#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>

#include "common.h"

#include "fifo_internal.h"
#include "fifo.h"

#define FIFO_MASK_WRITE 1
#define FIFO_MASK_READ  2

struct _fifo
{
	pthread_mutex_t lock;
	struct fifo_internal internal;
	int enable;
	xmpp_ctx_t *ctx;
};

fifo_t* fifo_new(xmpp_ctx_t *ctx)
{
	fifo_t *self = xmpp_alloc(ctx, sizeof(fifo_t));

	if (pthread_mutex_init(&self->lock, NULL)) {
		xmpp_error(ctx, "xmpp", "%s: pthread_mutex_init() failed: errno=%d", __func__, errno);
		return NULL;
	}

	self->ctx = ctx;
	fifo_internal_build(&self->internal, sizeof(void*));
	fifo_internal_set_descriptor(&self->internal, -1, -1);

	return self;
}

void fifo_delete(fifo_t *self)
{
	assert(self);

	if (self->enable & FIFO_MASK_WRITE)
		close(fifo_internal_get_write_descriptor(&self->internal));
	if (self->enable & FIFO_MASK_READ)
		close(fifo_internal_get_read_descriptor(&self->internal));

	xmpp_free(self->ctx, self);
}

int fifo_put(fifo_t *self, void *data)
{
	int ret = 0;

	assert(self);
	assert(data);

	//pthread_mutex_lock(&self->lock);

	if (self->enable & FIFO_MASK_WRITE)
		ret = fifo_internal_put(&self->internal, data);

	//pthread_mutex_unlock(&self->lock);

	return ret;
}

int fifo_get(fifo_t *self, void *data)
{
	int ret = 0;

	assert(self);

	if (self->enable & FIFO_MASK_READ) {
		ret = fifo_internal_get(&self->internal, data);

		if (!ret) {
			pthread_mutex_lock(&self->lock);
			close(fifo_internal_get_read_descriptor(&self->internal));
			self->enable &= ~FIFO_MASK_READ;
			pthread_mutex_unlock(&self->lock);
		}
	}

    return ret;
}

int fifo_get_read_descriptor(fifo_t *self)
{
	return fifo_internal_get_read_descriptor(&self->internal);
}

void fifo_set_nonblock(fifo_t *self)
{
	fifo_internal_set_nonblock(&self->internal);
}

void fifo_set_block(fifo_t *self)
{
	fifo_internal_set_block(&self->internal);
}

int fifo_is_enabled(fifo_t *self)
{
	return self->enable;
}

int fifo_enable(fifo_t *self)
{
	int pipes[2];
	if (pipe(pipes))
		xmpp_error(self->ctx, "xmpp", "%s: pipe() failed: errno=%d", __func__, errno);
	else {
		fifo_internal_set_descriptor(&self->internal, pipes[1], pipes[0]);
		self->enable = FIFO_MASK_READ | FIFO_MASK_WRITE;
	}
	return 0;
}

static int fifo_disable(fifo_t *self, int mask, int ds)
{
	int ret = -1;

	pthread_mutex_lock(&self->lock);

	if (!(self->enable & mask))
		goto err_enable;

	self->enable &= ~mask;
	close(ds);

	ret = 0;
err_enable:
	
	pthread_mutex_unlock(&self->lock);

	return ret;
}

int fifo_disable_writer(fifo_t *self)
{
	return fifo_disable(self, FIFO_MASK_WRITE, fifo_internal_get_write_descriptor(&self->internal));
}

int fifo_disable_reader(fifo_t *self) {
	return fifo_disable(self, FIFO_MASK_READ, fifo_internal_get_read_descriptor(&self->internal));
}

