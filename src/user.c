#include <pthread.h>

#include "strophe.h"
#include "antistrophe.h"
#include "common.h"

xmpp_user_t* xmpp_user_new(xmpp_ctx_t * const ctx, const char *jid, const char *name)
{
	xmpp_user_t *user;

	if (ctx == NULL)
		return NULL;
	
	user = xmpp_alloc(ctx, sizeof(xmpp_user_t));
	user->ctx = ctx;
	user->jid = xmpp_strdup(ctx, jid);
	user->name = xmpp_strdup(ctx, name);

	return user;
}

void xmpp_user_release(xmpp_user_t * const user)
{
	xmpp_ctx_t *ctx = user->ctx;

	if (user->name) xmpp_free(ctx, user->name);
	if (user->jid) xmpp_free(ctx, user->jid);
	xmpp_free(ctx, user);
}

xmpp_user_t** xmpp_get_users(xmpp_conn_t * const conn, int *len)
{
	*len = conn->users_len;
	return conn->users;
}

void xmpp_lock_users(xmpp_conn_t * const conn)
{
	pthread_mutex_lock(&conn->users_lock);
}

void xmpp_unlock_users(xmpp_conn_t * const conn)
{
	pthread_mutex_unlock(&conn->users_lock);
}



