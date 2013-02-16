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

#ifndef __TTX_PROVIDER_HELPERS_H__
#define __TTX_PROVIDER_HELPERS_H__

#include <glib.h>

#include <ttx-link.h>
#include <ttx-provider.h>

G_BEGIN_DECLS

/* some helper functions etc. for writing providers */

typedef void (*TTXLinkRemapFunc) (TTXLink *link);

/* callback data */
struct _TTXProviderCBData {
	char				*links_uri;	/* uri for linkpage */
	char				*links_path;	/* linkspage local path */
	char				*img_path;	/* img local path */
	char				*href_rx;	/* rx matching
							 * href in
							 * <area> */
	unsigned			 page, subpage;
	TTXLinkRemapFunc		 remap_func;
	TTXProviderResultFunc		 result_func;
	gpointer			 user_data;
};
typedef struct _TTXProviderCBData	 TTXProviderCBData;


/**
 * Create a structure for callback data
 *
 * @param img_uri URI to the page image
 * @param path the download path
 * @param page the page number
 * @param subpage the subpage number
 * @param func function to call with results
 * @param user_data user pointer passed to func
 *
 * @return a new TTXProviderCBData struct; free with
 *   ttx_provider_cb_data_destroy
 */
TTXProviderCBData* ttx_provider_cb_data_new (void);
/**
 * Destroy the callback data structure
 *
 * @param cbdata callback data
 */
void ttx_provider_cb_data_destroy (TTXProviderCBData *cbdata);


/**
 * Report a download failure (using the callbacks in CBData)
 *
 * @param cbdata callback data
 */
void ttx_provider_report_failure (TTXProviderCBData *cbdata);



G_END_DECLS

#endif /*__TTX_PROVIDER_HELPERS_H__*/
