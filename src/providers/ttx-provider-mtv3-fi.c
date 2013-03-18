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
<map name="100-01">
	<AREA SHAPE="RECT" COORDS="40,0,70,12" HREF="100-01.htm">
	<AREA SHAPE="RECT" COORDS="370,48,400,72" HREF="105-01.htm">
	<AREA SHAPE="RECT" COORDS="370,72,400,96" HREF="114-01.htm">
	<AREA SHAPE="RECT" COORDS="370,96,400,120" HREF="205-01.htm">
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
		 "http://www.mtv3tekstikanava.fi/new2008/images/%u-%02u.gif",
		 "http://www.mtv3tekstikanava.fi/new2008/%u-%02u.htm",
		 "(\\d{3})-(\\d{2}).*>",
		 dir, NULL,
		 func, user_data);
}


TTXProvider*
ttx_provider_mtv3_fi (void)
{
	return ttx_provider_new ("MTV3 Tekstikanava",
				 _("Finnish Teletekst Service"),
				 retrieve);
}
