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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <string.h>
#include <glib/gi18n-lib.h>

#include "ttx-provider.h"
#include "ttx-link.h"
#include "ttx-http.h"

struct _CBData {
	TTXProviderResultFunc func;
	char *html_uri;
	char *path;
	gpointer user_data;
	unsigned page, subpage;
};
typedef struct _CBData CBData;

static void
cb_data_destroy (CBData *cbdata)
{
	if (!cbdata)
		return;

	g_free (cbdata->path);
	g_free (cbdata->html_uri);

	g_free (cbdata);
}


static void
report_download_failed (CBData *cbdata)
{
	g_warning ("error getting %u/%u",
		   cbdata->page, cbdata->subpage);

	cbdata->func (TTX_RETRIEVAL_ERROR,
		      cbdata->page,
		      cbdata->subpage,
		      NULL, NULL,
		      cbdata->user_data);
}


/* map looks something like:
   <map name="P100_01">
   <area shape="rect" coords="80,-12,110,0" href="http://abc/P100_01.html"/>
   <area shape="rect" coords="230,36,260,48" href="http://abc/P199_01.html"/>
   ...
   </map>

   we process it with a regexp.
*/

#define AREA_REGEXP					\
	"<area.*"	                 		\
	"coords=\"(\\w+),(\\w+),(\\w+),(\\w+)\".*"      \
	"P(\\d{3})_(\\d{2}).*>"

static GSList*
process_map (const char *map)
{
	GRegex *rx;
	GError *err;
	GSList *lst;
	GMatchInfo *minfo;

	err = NULL;

	rx = g_regex_new (AREA_REGEXP,
			  G_REGEX_UNGREEDY|G_REGEX_OPTIMIZE|G_REGEX_CASELESS,
			  0,
			  &err);
	if (!rx) {
		g_warning ("error in regexp: %s",
			   err ? err->message : "something's wrong");
		g_clear_error (&err);
		return NULL;
	}

	lst = NULL;
	while (g_regex_match (rx, map, 0, &minfo)) {
		unsigned nums[6], u;
		for (u = 0; u != G_N_ELEMENTS(nums); ++u) {
			char *str;
			str = g_match_info_fetch (minfo, u + 1);
			nums[u] = atoi (str);
			g_free (str);
		}
		lst = g_slist_prepend (lst,
				       ttx_link_new (nums[0], nums[2],
						     nums[1], nums[3],
						     nums[4], nums[5]));

		map += strlen (g_match_info_fetch (minfo, 0));
		g_match_info_free (minfo);
	}

	g_regex_unref (rx);

	return lst;
}



#define MAP_BEGIN "<map "
#define MAP_END   "</map>"

static GSList*
get_links (const char *htmlpath)
{
	gboolean rv;
	GFile *file;
	GError *err;
	char *data, *b, *e;
	gsize len;
	GSList *lst;

	file = g_file_new_for_path (htmlpath);

	lst = NULL;
	err = NULL;
	rv = g_file_load_contents (file, NULL, &data, &len, NULL,
				   &err);
	if (!rv) {
	g_warning ("failed to load %s: %s", htmlpath,
			   err ? err->message : "something went wrong");
		g_clear_error (&err);
		goto leave;
	}

	/* now, find the <map ...> .... </map> part; ugh yuck
	 * screenscraping */
	b = g_strstr_len (data, -1, MAP_BEGIN);
	if (!b)
		goto leave;

	e = g_strstr_len (b, -1, MAP_END);
	if (!e)
		goto leave;
	e += strlen(MAP_END);
	e[0] = '\0';

	/* this should get us something like:
	<map name="P100_01">
	<area shape="rect" coords="80,-12,110,0" href="http://abc/P100_01.html"/>
	<area shape="rect" coords="230,36,260,48" href="http://abc/P199_01.html"/>
	...
	</map>
	*/
	lst = process_map (b);

leave:
	g_object_unref (file);
	g_free (data);

	return lst;
}




static void
on_got_html (TTXHTTPStatus hstatus, const char *htmlpath, CBData *cbdata)
{
	GSList *links;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		report_download_failed (cbdata);
		cb_data_destroy (cbdata);
		return;
	}

	/* parse html */
	links = get_links (htmlpath);

	cbdata->func (TTX_RETRIEVAL_OK,
		      cbdata->page, cbdata->subpage,
		      cbdata->path, /* the image path */
		      links,
		      cbdata->user_data);

	cb_data_destroy (cbdata);
}



static void
on_got_page (TTXHTTPStatus hstatus, const char *path, CBData *cbdata)
{
	char *htmlpath;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		report_download_failed (cbdata);
		cb_data_destroy (cbdata);
		return;
	}

	cbdata->path = g_strdup (path);
	htmlpath = g_strdup_printf ("%s.html", path);

	ttx_http_retrieve (cbdata->html_uri, htmlpath,
			   (TTXHTTPCompletedFunc)on_got_html,
			   cbdata);
	g_free (htmlpath);
}


gboolean
ttx_provider_type_2_retrieve (unsigned page, unsigned subpage,
			      const char *img_uri_frm, const char *html_uri_frm,
			      const char *dir, TTXProviderResultFunc func,
			      gpointer user_data)
{
	CBData *cbdata;
	char *page_uri, *path;

	g_return_val_if_fail (page >= 100 && page <= 999, FALSE);
	g_return_val_if_fail (subpage > 0, FALSE);
	g_return_val_if_fail (img_uri_frm, FALSE);
	g_return_val_if_fail (html_uri_frm, FALSE);
	g_return_val_if_fail (dir, FALSE);
	g_return_val_if_fail (func, FALSE);

	cbdata		  = g_new0 (CBData, 1);
	cbdata->func	  = func;
	cbdata->user_data = user_data;
	cbdata->page	  = page;
	cbdata->subpage	  = subpage;
	cbdata->path      = NULL;
	cbdata->html_uri  = g_strdup_printf (html_uri_frm, page, subpage);

	page_uri = g_strdup_printf (img_uri_frm, page, subpage);
	path = g_strdup_printf ("%s/%u-%u.gif", dir, page, subpage);

	ttx_http_retrieve (page_uri, path, (TTXHTTPCompletedFunc)on_got_page,
			   cbdata);
	g_free (page_uri);
	g_free (path);

	return TRUE;
}
