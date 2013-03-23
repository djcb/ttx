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
#include <glib/gi18n-lib.h>

#include "ttx-provider.h"
#include "ttx-provider-type-2.h"

/* map looks something like:
 *
 *<AREA SHAPE ="RECT" COORDS=" 370, 48, 400, 72" A
 *HREF="javascript:changeURL('bbc1','/txtmaster.php?page=107&subpage=0&channel=bbc1&search_string=&fontsize=2');"
 *onMouseOver="window.status='Goto page 107'; return true"
 *onMouseOut="window.status=''; return true">
 *
 *
 */

static gboolean
retrieve (unsigned page, unsigned subpage,
	  const char *dir, TTXProviderResultFunc func,
	  gpointer user_data)
{
	gboolean rv;
	char *imgurl;

	/* interestingly, the img-urls have 0-based subpage numbers.
	 * while the html urls are 1-based */
	imgurl = g_strdup_printf (
		"http://www.ceefax.tv/cgi-bin/gfx.cgi?"
		"page=%u_%u&font=big&channel=bbc1",
		page, subpage - 1);

	imgurl = g_strdup ("http://www.ceefax.tv/cgi-bin/gfx.cgi?page=100_0&font=big&channel=bbc1");

	rv = ttx_provider_type_2_retrieve
		(page, subpage,
		 imgurl,
		 "http://www.ceefax.tv/txtmaster.php?page=%u"
		 "&subpage=%u&channel=bbc1",
		 "page=(\\d{3}).*subpage=(\\d+)",
		 dir, NULL,
		 func, user_data);

	//g_free (imgurl);

	return rv;
}


TTXProvider*
ttx_provider_ceefax_uk (void)
{
	return ttx_provider_new ("Ceefax",
				 _("British Teletext Service"),
				 retrieve);
}
