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


static gboolean
retrieve (unsigned page, unsigned subpage,
	  const char *dir, TTXProviderResultFunc func,
	  gpointer user_data)
{
	gboolean rv;
	char *htm_uri, *png_uri;

	htm_uri = g_strdup_printf (
		"http://www.rtp.pt/wportal/fab-txt/%u/%%u_%%04u.htm",
		page);
	png_uri = g_strdup_printf (
		"http://www.rtp.pt/wportal/fab-txt/%u/%%u_%%04u.png",
		page);

	rv =  ttx_provider_type_2_retrieve
		(page, subpage, png_uri, htm_uri,
		 "/(\\d{3})_(\\d{4})\\.htm\"",
		 dir, NULL,
		 func, user_data);

	g_free (htm_uri);
	g_free (png_uri);

	return rv;
}


TTXProvider*
ttx_provider_rtp_pt (void)
{
	return ttx_provider_new ("RTP Teletexto",
				 _("Portuguese Teletekst Service"),
				 retrieve);
}
