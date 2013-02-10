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

#ifndef __TTX_PROVIDER_TYPE_1_H__
#define __TTX_PROVIDER_TYPE_1_H__

/**
 * Get page / links for a 'type 2' provider (for lack of a better name)
 *
 * @param page the page number
 * @param subpage the subpage number
 * @param img_uri_frm the image URI format-string
 *       (e.g., "http://www.yle.fi/tekstitv/images/P%u_%02u.gif")
 * @param html_uri_frm the URI format-string for the HTML-file with links info
 *       (e.g., "http://www.yle.fi/tekstitv/html/P%u_%02u.html")
 * @param path path for the image file for this page
 * @param func function to call with results
 * @param user_data user pointer passed to the result function
 *
 * @return TRUE if initiating the retrieval worked, FALSE otherwise
 */
gboolean
ttx_provider_type_2_retrieve (unsigned page, unsigned subpage,
			      const char *img_uri_frm, const char *html_uri_frm,
			      const char *dir, TTXProviderResultFunc func,
			      gpointer user_data);


#endif /*__TTX_PROVIDER_TYPE_1_H__*/
