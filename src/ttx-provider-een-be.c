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

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <string.h>

#include "ttx-link.h"

#include "ttx-provider-een-be.h"

#define PROVIDER_ID TTX_PROVIDER_EEN_BE


struct _CBData {
	TTXProviderResultFunc func;
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
	g_free (cbdata);
}


static void
report_download_failed (CBData *cbdata)
{
	g_warning ("error getting %u/%u",
		   cbdata->page, cbdata->subpage);

	cbdata->func (TTX_RETRIEVAL_ERROR,
		      PROVIDER_ID,
		      cbdata->page,
		      cbdata->subpage,
		      NULL, NULL,
		      cbdata->user_data);
}


static TTXLink*
parse_xml_link (xmlNodePtr node)
{
	xmlAttr *props;
	unsigned left, right, top, bottom, page, subpage;

	left = right = top = bottom = page = subpage = 0;

	for (props = node->properties; props; props = props->next) {

		const char *numstr, *prop;
		if (!props->children || !props->children->content)
			continue;

		prop   = (const char*)props->name;
		numstr = (const char*)props->children->content;

		if (g_strcmp0(prop, "left") == 0)
			left = atoi (numstr);
		else if (g_strcmp0(prop, "right") == 0)
			right = atoi (numstr);
		else if (g_strcmp0(prop, "top") == 0)
			top = atoi (numstr);
		else if (g_strcmp0(prop, "bottom") == 0)
			bottom = atoi (numstr);
		else if (g_strcmp0(prop, "link") == 0) {
			page = atoi (numstr + 1);
			numstr = strchr (numstr, '_');
			if (numstr)
				subpage = atoi (numstr + 1);
		}

	}

	if (left && right && top && bottom && page && subpage)
		return ttx_link_new (left, right, top, bottom,
				     page, subpage);
	else
		return NULL;
}




static GSList*
parse_xml_links (xmlNodeSetPtr nodeset)
{
	int i;
	GSList *lst;

	if (xmlXPathNodeSetIsEmpty (nodeset))
		return NULL;

	for (i = 0, lst = NULL ; i != nodeset->nodeNr; ++i) {
		TTXLink *link;

		link = parse_xml_link (nodeset->nodeTab[i]);
		if (link)
			lst = g_slist_prepend (lst, link);
	}

	return lst;
}


static GSList*
parse_xml (const char *path)
{
	xmlDocPtr doc;
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	GSList *lst1, *lst2;
	xmlChar *xpath;

	lst1	= lst2 = NULL;
	doc	= NULL;
	context = NULL;
	result	= NULL;

	if (!(doc = xmlParseFile (path))) {
		g_warning ("error parsing %s", path);
		goto leave;
	}

	if (!(context = xmlXPathNewContext (doc))) {
		g_warning ("failed to get path context for %s", path);
		goto leave;
	}

	xpath = (xmlChar*)"/root/page_links/page_link";
	if ((result = xmlXPathEvalExpression (xpath, context))) {
		lst1 = parse_xml_links (result->nodesetval);
		xmlXPathFreeObject (result);
	}

	xpath = (xmlChar*)"/root/fasttext_links/fasttext_link";
	if ((result = xmlXPathEvalExpression (xpath, context))) {
		lst2 = parse_xml_links (result->nodesetval);
		xmlXPathFreeObject (result);
	}

	/* merge */
	if (lst1)
		g_slist_last (lst1)->next = lst2;
	else
		lst1 = lst2;
leave:
	if (doc)
		xmlFreeDoc (doc);
	if (context)
		xmlXPathFreeContext (context);

	return lst1;
}




static void
on_got_xml (TTXHTTPStatus hstatus, const char *xmlpath, CBData *cbdata)
{
	GSList *links;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		report_download_failed (cbdata);
		cb_data_destroy (cbdata);
		return;
	}

	links = parse_xml (xmlpath);

	cbdata->func (TTX_RETRIEVAL_OK,
		      PROVIDER_ID,
		      cbdata->page, cbdata->subpage,
		      cbdata->path, /* the image path */
		      links,
		      cbdata->user_data);

	cb_data_destroy (cbdata);
}



static void
on_got_page (TTXHTTPStatus hstatus, const char *path, CBData *cbdata)
{
	char *links_uri, *xmlpath;

	/* fail */
	if (hstatus == TTX_HTTP_STATUS_ERROR) {
		report_download_failed (cbdata);
		cb_data_destroy (cbdata);
		return;
	}

	cbdata->path = g_strdup (path);

	/* got the page, now get the links... */
	links_uri = g_strdup_printf (
		"http://services.vrt.be/data/teletext/webpages-xml/P%u_%02u.xml",
		cbdata->page, cbdata->subpage);
	xmlpath = g_strdup_printf ("%s.xml", path);

	ttx_http_retrieve (links_uri, xmlpath,
			   (TTXHTTPCompletedFunc)on_got_xml,
			   cbdata);

	g_free (links_uri);
	g_free (xmlpath);
}


void
ttx_provider_een_be_retrieve (unsigned page, unsigned subpage,
			      const char *dir,
			      TTXProviderResultFunc func,
			      gpointer user_data)
{
	char *page_uri, *path;
	CBData *cbdata;

	g_return_if_fail (page >= 100 && page <= 999);
	g_return_if_fail (subpage > 0);
	g_return_if_fail (dir);
	g_return_if_fail (func);

	cbdata = g_new0 (CBData, 1);
	cbdata		  = g_new0 (CBData, 1);
	cbdata->func	  = func;
	cbdata->user_data = user_data;
	cbdata->page	  = page;
	cbdata->subpage	  = subpage;
	cbdata->path      = NULL;

	page_uri  = g_strdup_printf (
		"http://services.vrt.be/data/teletext/webpages-images/P%u_%02u.gif",
		page, subpage);

	path = g_strdup_printf ("%s/%u-%u-%u.gif", dir,
				PROVIDER_ID, page, subpage);

	ttx_http_retrieve (page_uri, path, (TTXHTTPCompletedFunc)on_got_page,
			   cbdata);

	g_free (page_uri);
	g_free (path);
}
