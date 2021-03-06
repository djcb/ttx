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
 * A new provider has to add two things here:
 * 1) add a declaration of the function to create a TTXProvider*
 *    of the new kind
 * 2) connect the ID and the function by adding an entry
 *    to TTX_PROVIDERS
 */

TTXProvider*	ttx_provider_ceefax_uk  (void);
TTXProvider*	ttx_provider_een_be     (void);
TTXProvider*	ttx_provider_eins_de    (void);
TTXProvider*	ttx_provider_hrt_hr     (void);
TTXProvider*	ttx_provider_mtv3_fi    (void);
TTXProvider*	ttx_provider_nos_nl     (void);
TTXProvider*	ttx_provider_orf_at     (void);
TTXProvider*	ttx_provider_rai_it     (void);
TTXProvider*	ttx_provider_rtp_pt     (void);
TTXProvider*	ttx_provider_rtve_es    (void);
TTXProvider*	ttx_provider_rtvslo_si  (void);
TTXProvider*	ttx_provider_yle_fi     (void);



typedef const char*  TTXProviderID;
typedef TTXProvider* (*TTXProviderFunc) (void);

static const struct {
	TTXProviderID     prov_id;
	TTXProviderFunc   new_func;
} TTX_PROVIDERS[] = {
	/* for now, disable Ceefax */
	/* { "cfx",        ttx_provider_ceefax_uk }, */
	{ "eins",       ttx_provider_eins_de   },
	/* { "hrt",        ttx_provider_hrt_hr    }, */
	{ "mtv3",       ttx_provider_mtv3_fi   },
	{ "nos",        ttx_provider_nos_nl    },
	{ "orf",        ttx_provider_orf_at    },
	{ "rai",        ttx_provider_rai_it    },
	{ "rtp",        ttx_provider_rtp_pt    },
	{ "rtve",       ttx_provider_rtve_es   },
	{ "rtvslo",     ttx_provider_rtvslo_si },
	{ "vrt",        ttx_provider_een_be    },
	{ "yle",        ttx_provider_yle_fi    }

};


#endif /*__TTX_PROVIDERS_H__*/
