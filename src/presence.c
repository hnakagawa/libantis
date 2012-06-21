#include "antistrophe.h"
#include "common.h"
#include "util.h"

void xmpp_send_presence(xmpp_conn_t * const conn, xmpp_presence_show_t sw, const char *sv)
{
	static const char *show_table[] = {
		"chat",
		"away",
		"xa",
		"dnd"
	};
	xmpp_stanza_t *presence, *show, *status, *text;

	presence = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(presence, "presence");

	show = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(show, "show");
	xmpp_stanza_add_child(presence, show);

	text = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_text(text, show_table[sw]);
	xmpp_stanza_add_child(show, text);
	xmpp_stanza_release(text);

	xmpp_stanza_release(show);

	status = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_add_child(presence, status);

	text = xmpp_stanza_new(conn->ctx);
	xmpp_stanza_set_text(text, sv);
	xmpp_stanza_add_child(status, text);
	xmpp_stanza_release(text);

	xmpp_stanza_release(status);

	xmpp_send(conn, presence);
	xmpp_stanza_release(presence);
}

