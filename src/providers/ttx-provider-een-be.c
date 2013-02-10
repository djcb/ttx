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
#include "ttx-provider-type-1.h"

static gboolean
retrieve (unsigned page, unsigned subpage,
	  const char *dir, TTXProviderResultFunc func,
	  gpointer user_data)
{
	return ttx_provider_type_1_retrieve
		(page, subpage,
		 "http://services.vrt.be/data/teletext/webpages-images/P%u_%02u.gif",
		 "http://services.vrt.be/data/teletext/webpages-xml/P%u_%02u.xml",
		 dir, func, user_data);
}

TTXProvider*
ttx_provider_een_be (void)
{
	return ttx_provider_new ("Een BE Teletekst",
				 _("Flemish Teletekst Service"),
				 retrieve);
}
