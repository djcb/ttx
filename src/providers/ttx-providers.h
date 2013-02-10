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

#ifndef __TTX_PROVIDERS_H__
#define __TTX_PROVIDERS_H__

#include <ttx-provider.h>

/**
 * A new provider has to add three things here:
 * 1) add an ID for it in the TTXProviderID enum
 * 2) add a declaration of the function to create a TTXProvider*
 *    of the new kind
 * 3) connect the ID and the function by adding an entry
 *    to TTX_PROVIDERS
 */

/** enumeration of provider ids */
typedef enum {
	TTX_PROVIDER_NOS_TELETEKST  = 0, /**< NOS Teletekst */
	TTX_PROVIDER_EEN_BE,             /**< VRT Teletekst */

	TTX_PROVIDER_NUM                 /**< number of providers */
} TTXProviderID;


TTXProvider* ttx_provider_nos_teletekst (void);
TTXProvider* ttx_provider_een_be (void);

typedef TTXProvider* (*TTXProviderFunc) (void);

static const struct {
	TTXProviderID     id;
	TTXProviderFunc   new_func;
} TTX_PROVIDERS[] = {
	{ TTX_PROVIDER_NOS_TELETEKST, ttx_provider_nos_teletekst },
	{ TTX_PROVIDER_EEN_BE,        ttx_provider_een_be }
};


#endif /*__TTX_PROVIDERS_H__*/
