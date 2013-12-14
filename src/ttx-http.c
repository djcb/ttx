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

#include <config.h>

#include <libsoup/soup.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "ttx-http.h"

struct _CBData {
	TTXHTTPCompletedFunc	 func;
	gpointer		 user_data;
	char			*path;
	GFile                   *file;
        GFileOutputStream       *ostream;
        GInputStream            *istream;
};
typedef struct _CBData CBData;

static void
free_cbdata (CBData *cbdata)
{
	g_clear_object (&cbdata->file);
	g_clear_object (&cbdata->istream);
	g_clear_object (&cbdata->ostream);

	g_free (cbdata->path);

	g_free (cbdata);
}

static void
on_spliced (GOutputStream *ostream, GAsyncResult *res, CBData *cbdata)
{
        GError          *err;
        gboolean         rv;
        
        err = NULL;
        rv = g_output_stream_splice_finish (ostream, res, &err);
        if (!rv) {
		g_warning ("download: %s", err ? err->message:
			   "something went wrong");
		cbdata->func (TTX_HTTP_STATUS_ERROR, NULL,
			      cbdata->user_data);
	} else
		cbdata->func (TTX_HTTP_STATUS_COMPLETED,
			      cbdata->path, cbdata->user_data);	
		
	 free_cbdata (cbdata);
}	
 



static void
on_session (SoupSession *session, GAsyncResult *res, CBData *cbdata)
{
	GError *err;

	err = NULL;

	cbdata->istream = soup_session_send_finish (session, res, &err);
        if (!cbdata->istream)
                goto errexit;

        cbdata->file	= g_file_new_for_path (cbdata->path);
	/* fails if file does not exist, that's fine */
	unlink (cbdata->path); 
        cbdata->ostream = g_file_create (cbdata->file,
					 G_FILE_CREATE_REPLACE_DESTINATION |
					 G_FILE_CREATE_PRIVATE, NULL, &err);
        if (!cbdata->ostream)
                goto errexit;
	
	 g_output_stream_splice_async (G_OUTPUT_STREAM(cbdata->ostream),
				       cbdata->istream,
				       G_OUTPUT_STREAM_SPLICE_CLOSE_SOURCE |
				       G_OUTPUT_STREAM_SPLICE_CLOSE_TARGET,
				       G_PRIORITY_DEFAULT, NULL,
				       (GAsyncReadyCallback)on_spliced,
				       cbdata);
	 return;

errexit:
	g_warning ("creating i/o streams failed: %s",
		  err ? err->message : "something went wrong");
	g_clear_error (&err);
	cbdata->func (TTX_HTTP_STATUS_ERROR, NULL,
		       cbdata->user_data);
	free_cbdata (cbdata);
}

void
ttx_http_retrieve (const char *uri, const char *local_path,
		   TTXHTTPCompletedFunc func, gpointer user_data)
{
	SoupMessage	*msg;
	SoupMessageBody *mbody;
	SoupSession	*session;
	CBData		*cbdata;

	g_return_if_fail (uri);
	g_return_if_fail (local_path);
	g_return_if_fail (func);

	msg	= soup_message_new ("GET", uri);
	g_object_get (G_OBJECT(msg), "response-body", &mbody, NULL);

        soup_message_body_set_accumulate (mbody, FALSE);
        soup_message_body_free (mbody);


	session = soup_session_new_with_options (
		"timeout", 8, 
		"user-agent", PACKAGE_NAME "/" PACKAGE_VERSION,
		NULL);

	cbdata		  = g_new0 (CBData, 1);

	cbdata->func	  = func;
	cbdata->user_data = user_data;
	cbdata->path      = g_strdup (local_path);

	/* the callback data will be freed when the message is */
	g_object_set_data_full (G_OBJECT(msg), "cbdata", cbdata,
				(GDestroyNotify)free_cbdata);

	soup_session_send_async (session, msg, NULL,
				 (GAsyncReadyCallback)on_session,
				 cbdata);
	g_object_unref (session);
}
