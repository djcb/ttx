/*
** Copyright (C) 2013 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 3, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation,
** Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**
*/

#include <libsoup/soup.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "ttx-http.h"

struct _CBData {
	TTXHTTPCompletedFunc	 func;
	gpointer		 user_data;
	char			*path;
};
typedef struct _CBData CBData;

static void
free_cbdata (CBData *cbdata)
{
	g_free (cbdata->path);
	g_free (cbdata);
}

static gboolean
save_message (SoupMessage *msg, const char *path)
{
	FILE *file;
	SoupBuffer *buf;
	size_t written;
	gboolean rv;
	SoupMessageBody *body;

	file = fopen (path, "w");
	if (!file) {
		g_warning ("failed to open %s for writing: %s",
			   path, strerror (errno));
		return FALSE;
	}

	g_object_get (G_OBJECT(msg), "response-body", &body, NULL);
	buf = soup_message_body_flatten (body);

	written	= fwrite (buf->data, buf->length, 1, file);
	if (written != 1) {
		g_warning ("error writing %s: %s", path,
			   strerror (errno));
		rv = FALSE;
	} else
		rv = TRUE;

	soup_buffer_free (buf);
	soup_message_body_free (body);

	fclose (file);

	return rv;
}


static void
on_session (SoupSession *session, SoupMessage *msg, CBData *cbdata)
{
	if (msg->status_code != SOUP_STATUS_OK) {
		char *uristr;
		uristr = soup_uri_to_string
			(soup_message_get_uri (msg), FALSE);
		g_warning ("retrieving %s failed: %s",
			   uristr,
			   msg->reason_phrase ? msg->reason_phrase :
			   "something went wrong");
		g_free (uristr);
		cbdata->func (TTX_HTTP_STATUS_ERROR, NULL,
			      cbdata->user_data);
		return;
	}

	if (!save_message (msg, cbdata->path))
		cbdata->func (TTX_HTTP_STATUS_ERROR, NULL,
			      cbdata->user_data);

	cbdata->func (TTX_HTTP_STATUS_COMPLETED, cbdata->path,
		      cbdata->user_data);
}

void
ttx_http_retrieve (const char *uri, const char *local_path,
		   TTXHTTPCompletedFunc func, gpointer user_data)
{
	SoupMessage *msg;
	SoupSession *session;
	CBData *cbdata;

	g_return_if_fail (uri);
	g_return_if_fail (local_path);
	g_return_if_fail (func);

	msg	= soup_message_new ("GET", uri);
	session = soup_session_async_new ();

	cbdata		  = g_new0 (CBData, 1);

	cbdata->func	  = func;
	cbdata->user_data = user_data;
	cbdata->path      = g_strdup (local_path);

	/* the callback data will be freed when the message is */
	g_object_set_data_full (G_OBJECT(msg), "cbdata", cbdata,
				(GDestroyNotify)free_cbdata);

	soup_session_queue_message (session, msg,
				    (SoupSessionCallback)on_session,
				    cbdata);
}
