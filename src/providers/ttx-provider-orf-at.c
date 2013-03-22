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
   <map name="P100_01">
   <area shape="rect" coords="80,-12,110,0" href="http://abc/P100_01.html"/>
   <area shape="rect" coords="230,36,260,48" href="http://abc/P199_01.html"/>
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
		 "http://teletext.orf.at/100/%u_%04u.png",
		 "http://teletext.orf.at/100/%u_%04u.htm",
		 "(\\d{3})_(\\d{4})\\.htm.*>",
		 dir, NULL,
		 func, user_data);
}


TTXProvider*
ttx_provider_orf_at (void)
{
	return ttx_provider_new ("ORF Teletext",
				 _("Austrian Teletekst Service"),
				 retrieve);
}
