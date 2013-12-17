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
#include <gio/gio.h>

#define GNU_SOURCE
#include <string.h>
#include <glib/gi18n-lib.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "ttx-provider.h"
#include "ttx-link.h"
#include "ttx-http.h"
#include "ttx-provider-helpers.h"


static GSList*
process_map (const char *map, const char *href_rx,
	     TTXLinkRemapFunc remap_func)
{
	GRegex *rx;
	GError *err;
	GSList *lst;
	GMatchInfo *minfo;
	char *area_rx;

	err	= NULL;
	area_rx = g_strconcat (
		"<area.*"
		"coords=\"\\s*(\\w+)\\s*,\\s*(\\w+)\\s*,"
		"\\s*(\\w+)\\s*,\\s*(\\w+)\\s*\".*",
		href_rx, NULL);

	rx = g_regex_new (area_rx,
			  G_REGEX_UNGREEDY|G_REGEX_OPTIMIZE|G_REGEX_CASELESS,
			  0, &err);
	g_free (area_rx);
	if (!rx) {
		g_warning ("error in regexp: %s",
			   err ? err->message : "something went wrong");
		g_clear_error (&err);
		return NULL;
	}

	lst = NULL;
	while (g_regex_match (rx, map, 0, &minfo)) {
		unsigned nums[6], u;
		char *str;
		TTXLink *link;

		for (u = 0; u != G_N_ELEMENTS(nums); ++u) {
			char *str;
			str = g_match_info_fetch (minfo, u + 1);
			nums[u] = atoi (str);
			g_free (str);
		}

		/* some providers don't use subpages; we
		 * just us a pseudo subpage 1 */
		if (nums[5] == 0)
			nums[5] = 1;

		link = ttx_link_new (nums[0], nums[2],nums[1], nums[3],
				     nums[4], nums[5]);
		/* for some providers (RAI), the coordinates require
		 * massaging */
		if (remap_func)
			remap_func (link);

		lst = g_slist_prepend (lst, link);
		str = g_match_info_fetch (minfo, 0);
		map += strlen (str);
		g_free (str);
		g_match_info_free (minfo);
	}

	g_match_info_free (minfo);
	g_regex_unref (rx);

	return lst;
}



#define MAP_BEGIN "<map "
#define MAP_END   "</map>"

static GSList*
get_links (const char *htmlpath, const char *href_rx,
	   TTXLinkRemapFunc remap_func)
{
	gboolean	 rv;
	GFile		*file;
	GError		*err;
	char		*data, *b, *e;
	gsize		 len;
	GSList		*lst;

	file = g_file_new_for_path (htmlpath);

	data = NULL;
	lst  = NULL;
	err  = NULL;
	rv   = g_file_load_contents (file, NULL, &data, &len, NULL,
				   &err);
	if (!rv) {
		g_warning ("failed to load %s: %s", htmlpath,
			   err ? err->message : "something went wrong");
		g_clear_error (&err);
		goto leave;
	}

	/* now, find the <map ...> .... </map> part; ugh yuck
	 * screenscraping */
	b = strcasestr (data, MAP_BEGIN);
	if (!b)
		goto leave;

	e = strcasestr (b, MAP_END);
	if (!e)
		goto leave;

	e += strlen(MAP_END);
	e[0] = '\0';

	/* this should yield us something like (for YLE):
	<map name="P100_01">
	<area shape="rect" coords="80,-12,110,0" href="http://abc/P100_01.html"/>
	<area shape="rect" coords="230,36,260,48" href="http://abc/P199_01.html"/>
	...
	</map>
	*/
	lst = process_map (b, href_rx, remap_func);
	
leave:
	g_object_unref (file);
	g_free (data);

	return lst;
}




static void
on_got_links (TTXHTTPStatus hstatus, const char *htmlpath,
	     TTXProviderCBData *cbdata)
{
	GSList *links;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		ttx_provider_report_failure (cbdata);
		ttx_provider_cb_data_destroy (cbdata);
		return;
	}

	/* parse html */
	links = get_links (htmlpath, cbdata->href_rx, cbdata->remap_func);

	cbdata->result_func (TTX_RETRIEVAL_OK,
			     cbdata->page, cbdata->subpage,
			     cbdata->img_path, /* the image path */
			     links,
			     cbdata->user_data);

	ttx_provider_cb_data_destroy (cbdata);

	if (remove (htmlpath) != 0)
		g_warning ("failed to unlink %s: %s",
			  htmlpath, strerror (errno));
}



static void
on_got_img (TTXHTTPStatus hstatus, const char *path,
	     TTXProviderCBData *cbdata)
{
	char *htmlpath;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		ttx_provider_report_failure (cbdata);
		ttx_provider_cb_data_destroy (cbdata);
		return;
	}

	cbdata->img_path = g_strdup (path);
	htmlpath = g_strdup_printf ("%s.html", path);

	ttx_http_retrieve (cbdata->links_uri, htmlpath,
			   (TTXHTTPCompletedFunc)on_got_links,
			   cbdata);
	g_free (htmlpath);
}


gboolean
ttx_provider_type_2_retrieve (unsigned page, unsigned subpage,
			      const char *img_uri_frm,
			      const char *links_uri_frm,
			      const char *href_rx,
			      const char *dir,
			      TTXLinkRemapFunc remap_func,
			      TTXProviderResultFunc result_func,
			      gpointer user_data)
{
	TTXProviderCBData *cbdata;
	char *img_uri, *path;

	g_return_val_if_fail (page >= 100 && page <= 999, FALSE);
	g_return_val_if_fail (img_uri_frm, FALSE);
	g_return_val_if_fail (links_uri_frm, FALSE);
	g_return_val_if_fail (dir, FALSE);
	g_return_val_if_fail (result_func, FALSE);

	cbdata		    = ttx_provider_cb_data_new ();
	cbdata->result_func = result_func;
	cbdata->remap_func  = remap_func;
	cbdata->user_data   = user_data;
	cbdata->page	    =
		page;
	cbdata->subpage	    = subpage;
	cbdata->href_rx	    = g_strdup (href_rx);

	cbdata->links_uri = g_strdup_printf (links_uri_frm, page, subpage);
	img_uri = g_strdup_printf (img_uri_frm, page, subpage);
	path = g_strdup_printf ("%s/%u-%u.img", dir, page, subpage);

	ttx_http_retrieve (img_uri, path, (TTXHTTPCompletedFunc)on_got_img,
			   cbdata);
	g_free (img_uri);
	g_free (path);

	return TRUE;
}
