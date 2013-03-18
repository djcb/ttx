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
	<AREA SHAPE="RECT" COORDS="13,384,156,400" href="?str=110-01" alt="110" title="110">
	<AREA SHAPE="RECT" COORDS="169,384,286,400" href="?str=150-01" alt="150" title="150">
	<AREA SHAPE="RECT" COORDS="299,384,429,400" href="?str=300-01" alt="300" title="300">
	<AREA SHAPE="RECT" COORDS="442,384,520,400" href="?str=500-01" alt="500" title="500">
	<AREA SHAPE="RECT" COORDS="208,0,247,16" href="?str=100-01" alt="100" title="100">
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
		 "http://teletekst1.hrt.hr/images/%u-%02u.gif",
		 "http://teletekst.hrt.hr/?str=%u-%02u",
		 "(\\d{3})-(\\d{2}).*>",
		 dir, NULL,
		 func, user_data);
}


TTXProvider*
ttx_provider_hrt_hr (void)
{
	return ttx_provider_new ("HRT Teletekst",
				 _("Croation Teletekst Service"),
				 retrieve);
}
