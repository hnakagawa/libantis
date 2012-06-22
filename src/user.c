#include <pthread.h>

#include "strophe.h"
#include "antistrophe.h"
#include "common.h"

xmpp_user_t* xmpp_user_new(xmpp_conn_t * const conn, const char *jid, const char *name)
{
	xmpp_user_t *user = xmpp_alloc(conn->ctx, sizeof(xmpp_user_t));
	user->conn = conn;
	user->jid = xmpp_strdup(conn->ctx, jid);
	user->name = xmpp_strdup(conn->ctx, name);
	return user;
}

void xmpp_user_release(xmpp_user_t * const user)
{
	xmpp_ctx_t *ctx = user->conn->ctx;

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



