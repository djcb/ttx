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

#ifndef __TTX_HTTP_H__
#define __TTX_HTTP_H__

#include <glib.h>

G_BEGIN_DECLS


typedef enum {
	TTX_HTTP_STATUS_COMPLETED,
	TTX_HTTP_STATUS_ERROR
} TTXHTTPStatus;


typedef void (*TTXHTTPCompletedFunc) (TTXHTTPStatus status,
				      const char *path,
				      gpointer user_data);

/**
 * Dowload URI to local file, and report result in callback
 *
 * @param uri the URI to retrieve
 * @param local_path local path for the file to save
 * @param func callback function to call with results
 * @param user_data user pointer passed to callback
 */
void ttx_http_retrieve (const char *uri, const char *local_path,
			TTXHTTPCompletedFunc func, gpointer user_data);

G_END_DECLS
#endif /*__TTX_HTTP_H__*/
