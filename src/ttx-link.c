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

#include "ttx-link.h"


TTXLink*
ttx_link_new (unsigned left,
	      unsigned right,
	      unsigned top,
	      unsigned bottom,
	      unsigned page,
	      unsigned subpage)
{

	TTXLink *link;

	g_return_val_if_fail (page < 999, NULL);
	g_return_val_if_fail (subpage < 999, NULL);


	link = g_slice_new (TTXLink);

	link->left    = left;
	link->right   = right;
	link->top     = top;
	link->bottom  = bottom;
	link->page    = page;
	link->subpage = subpage;

	return link;
}


void
ttx_link_destroy (TTXLink *link)
{
	if (!link)
		return;

	g_slice_free (TTXLink, link);
}
