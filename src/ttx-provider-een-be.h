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

#ifndef __TTX_PROVIDER_EEN_BE_H__
#define __TTX_PROVIDER_EEN_BE_H__

#include "ttx-provider-mgr.h"

void ttx_provider_een_be_retrieve (unsigned page, unsigned subpage,
				   const char *dir,
				   TTXProviderResultFunc func,
				   gpointer user_data);

#endif /*__TTX_PROVIDER_EEN_BE_H__*/
