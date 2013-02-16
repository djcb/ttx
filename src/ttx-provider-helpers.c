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

#include "ttx-provider-helpers.h"

TTXProviderCBData*
ttx_provider_cb_data_new (void)
{
	TTXProviderCBData *cbdata;

	cbdata = g_new0 (TTXProviderCBData, 1);

	return cbdata;
}


void
ttx_provider_cb_data_destroy (TTXProviderCBData *cbdata)
{
	if (!cbdata)
		return;

	g_free (cbdata->links_path);
	g_free (cbdata->links_uri);
	g_free (cbdata->img_path);
	g_free (cbdata->href_rx);

	g_free (cbdata);
}


void
ttx_provider_report_failure (TTXProviderCBData *cbdata)
{
	g_return_if_fail (cbdata);

	g_warning ("error getting %u/%u",
		   cbdata->page, cbdata->subpage);

	cbdata->result_func (TTX_RETRIEVAL_ERROR,
			     cbdata->page,
			     cbdata->subpage,
			     NULL, NULL,
			     cbdata->user_data);
}
