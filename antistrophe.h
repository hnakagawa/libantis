#ifndef __LIBANTISTROPHE_H_
#define __LIBANTISTROPHE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	XMPP_SUCCESS,
	XMPP_ERROR
} xmpp_request_status_t;

typedef struct _xmpp_user_t xmpp_user_t;

struct _xmpp_user_t
{
	char *jid;
	char *name;
	xmpp_ctx_t *ctx;
};

typedef int (*xmpp_roster_handler)(xmpp_conn_t * const conn, xmpp_request_status_t status);

void xmpp_roster(xmpp_conn_t * const conn, xmpp_roster_handler handler);

xmpp_user_t* xmpp_user_new(xmpp_ctx_t * const ctx, const char *jid, const char *name);

void xmpp_user_release(xmpp_user_t * const user);

xmpp_user_t** xmpp_get_users(xmpp_conn_t * const conn, int *len);

void xmpp_lock_users(xmpp_conn_t * const conn);

void xmpp_unlock_users(xmpp_conn_t * const conn);
	
#ifdef __cplusplus
}
#endif

#endif
