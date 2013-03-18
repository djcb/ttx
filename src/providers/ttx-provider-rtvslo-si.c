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
<map name="110_000">
<area shape="RECT" coords="444,14,479,28"
href="/ttx/applications/ttx?cmd=load&page=112&subpage=0001">
<area shape="RECT" coords="444,28,479,42"
href="/ttx/applications/ttx?cmd=load&page=113&subpage=0001">
      ...
   </map>
*/

static gboolean
retrieve (unsigned page, unsigned subpage,
	  const char *dir, TTXProviderResultFunc func,
	  gpointer user_data)
{
	return ttx_provider_type_2_retrieve
		(page, subpage,
		 "http://teletext.rtvslo.si/ttx/%u_%04u.png",
		 "http://teletext.rtvslo.si/ttx/applications/ttx?cmd=load"
		 "&page=%u&subpage=%04u",
		 "page=(\\d{3})&subpage=(\\d{4}).*>",
		 dir, NULL,
		 func, user_data);
}

TTXProvider*
ttx_provider_rtvslo_si (void)
{
	return ttx_provider_new ("RTVSlo Teletekst",
				 _("Slovenian Teletekst Service"),
				 retrieve);
}
