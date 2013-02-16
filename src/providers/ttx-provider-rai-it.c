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
#include "ttx-provider-helpers.h"
#include "ttx-provider-type-2.h"


/* we need to remap the coordinates.... */
static void
link_remap (TTXLink *link)
{
	double x_ratio;

	x_ratio = 644.0 / 360.0;

 	link->left  = (double)link->left  * x_ratio;
	link->right = (double)link->right * x_ratio;
}



/* the map/area looks something like: rotator1.addMapArea('<map
   name="map1"><area COORDS="333,128,358,143"
   href="popupTelevideo.jsp?p=121&s=0&r=Nazionale">... */

static gboolean
retrieve (unsigned page, unsigned subpage,
	  const char *dir, TTXProviderResultFunc func,
	  gpointer user_data)
{
	const char* img_uri, *page_uri;

	/* weird special case... */
	if (subpage == 1) {
		img_uri = "http://www.televideo.rai.it/televideo/pub/tt4web/"
			"Nazionale/16_9_page-%u.png";
		page_uri =  "http://www.televideo.rai.it/televideo"
			"/pub/pagina.jsp?p=%u";
	} else {
		img_uri = "http://www.televideo.rai.it/televideo/pub/tt4web/"
			"Nazionale/16_9_page-%u.%u.png";
		page_uri =  "http://www.televideo.rai.it/televideo"
			"/pub/pagina.jsp?p=%u&s=%u"
			"&r=Nazionale&idmenumain=2&submit=VAI"
			"&pagetocall=pagina.jsp";
		/* without the &r=, &submit= etc. the website throws
		 * up some Java nullptr exception... Oops! */
	}

	return ttx_provider_type_2_retrieve
		(page, subpage, /* RAI subpage are 0-based */
		 img_uri, page_uri,
		 "\\?p=(\\d{3})&s=(\\d{1,2}).*",
		 dir, link_remap, func, user_data);
}

TTXProvider*
ttx_provider_rai_it (void)
{
	return ttx_provider_new ("RAI Televideo",
				 _("Italian Teletekst Service"),
				 retrieve);
}
