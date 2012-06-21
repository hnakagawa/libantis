#include <stdlib.h>
#include <pthread.h>

#include "antistrophe.h"

#include "common.h"
#include "util.h"

#define MAX_USER 1000

typedef struct {
	xmpp_request_handler handler;
	void *data;
} handler_userdata_t;

static int roster_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    xmpp_stanza_t *query, *item;
    char *type;
	handler_userdata_t *handler_userdata = userdata;
	int count;

    type = xmpp_stanza_get_type(stanza);
    if (!strcmp(type, "error")) {
	    xmpp_error(conn->ctx, "log", "ERROR: query failed");
		handler_userdata->handler(conn, XMPP_ERROR, handler_userdata->data);
		return 0;
	}

	pthread_mutex_lock(&conn->users_lock);

	conn->users = xmpp_alloc(conn->ctx, sizeof(xmpp_user_t*) * MAX_USER);
	
	query = xmpp_stanza_get_child_by_name(stanza, "query");
	for (count = 0, item = xmpp_stanza_get_children(query); item && count < MAX_USER;
		 item = xmpp_stanza_get_next(item), ++count) {

		conn->users[count] = xmpp_user_new(conn->ctx, xmpp_stanza_get_attribute(item, "jid"),
										   xmpp_stanza_get_attribute(item, "name"));
	}

	conn->users_len = count;

	pthread_mutex_unlock(&conn->users_lock);

	handler_userdata->handler(conn, XMPP_SUCCESS, handler_userdata->data);

	return 0;
}

void xmpp_send_roster(xmpp_conn_t * const conn, xmpp_request_handler handler, void * const userdata)
{
	xmpp_stanza_t *iq, *query;
	handler_userdata_t *handler_userdata;

	iq = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "get");
	xmpp_stanza_set_id(iq, "roster1");

	query = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_ns(query, XMPP_NS_ROSTER);

	xmpp_stanza_add_child(iq, query);

	/* we can release the stanza since it belongs to iq now */
	xmpp_stanza_release(query);

	handler_userdata = xmpp_alloc(conn->ctx, sizeof(handler_userdata_t));
	handler_userdata->handler = handler;
	handler_userdata->data = userdata;
	handler_add_id(conn, roster_handler, "roster1", handler_userdata, 1);

	/* send out the stanza */
	xmpp_send(conn, iq);

	/* release the stanza */
	xmpp_stanza_release(iq);
}
