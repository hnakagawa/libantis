#include "antistrophe.h"
#include "common.h"
#include "util.h"

void xmpp_send_message(xmpp_conn_t * const conn, const char *msg, const char *to)
{
	xmpp_stanza_t *message, *body, *text;

	message = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_attribute(message, "to", to);
	xmpp_stanza_set_attribute(message, "from", conn->jid);

	xmpp_stanza_set_type(message, "chat");

	body = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(body, "body");
	xmpp_stanza_set_text(body, msg);
	xmpp_stanza_add_child(message, body);

	text = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_text(text, msg);
	xmpp_stanza_add_child(body, text);
	xmpp_stanza_release(text);

	xmpp_stanza_release(body);

	xmpp_send(conn, message);
	xmpp_stanza_release(message);
}


