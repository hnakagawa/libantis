#ifndef __LIBANTISTROPHE_H_
#define __LIBANTISTROPHE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "strophe.h"

typedef enum {
	XMPP_SUCCESS,
	XMPP_ERROR
} xmpp_request_status_t;

typedef enum {
	XMPP_CHAT,
	XMPP_AWAY,
	XMPP_XA,
	XMPP_DND
} xmpp_presence_show_t;

typedef struct _xmpp_user_t xmpp_user_t;

struct _xmpp_user_t
{
	char *jid;
	char *name;
	xmpp_ctx_t *ctx;
};

typedef int (*xmpp_request_handler)(xmpp_conn_t * const conn, xmpp_request_status_t status, void * const userdata);

void xmpp_send_roster(xmpp_conn_t * const conn, xmpp_request_handler handler, void * const userdata);

void xmpp_send_presence(xmpp_conn_t * const conn, xmpp_presence_show_t sw, const char *sv);

void xmpp_send_message(xmpp_conn_t * const conn, const char *msg, const char *to);

xmpp_user_t* xmpp_user_new(xmpp_ctx_t * const ctx, const char *jid, const char *name);

void xmpp_user_release(xmpp_user_t * const user);

xmpp_user_t** xmpp_get_users(xmpp_conn_t * const conn, int *len);

void xmpp_lock_users(xmpp_conn_t * const conn);

void xmpp_unlock_users(xmpp_conn_t * const conn);

#ifdef __cplusplus
}
#endif

#endif
