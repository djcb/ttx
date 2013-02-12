/*
** Copyright (C) 2013 Dirk-Jan C. Binnema
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

#ifndef __TTX_PROVIDER_MGR_H__
#define __TTX_PROVIDER_MGR_H__

#include <ttx-http.h>
#include <ttx-provider.h>
#include <providers/ttx-providers.h>

struct _TTXProviderMgr;
typedef struct _TTXProviderMgr TTXProviderMgr;


/**
 * Instantiate a TTXProviderMgr
 *
 *
 * @return a new TTXProviderMgr or NULL in case of error; free with
 * ttx_provider_mgr_destroy
 */
TTXProviderMgr *ttx_provider_mgr_new (void);

/**
 * Free a TTXProviderMgr
 *
 * @param self a TTXProviderMgr, or NULL
 */
void ttx_provider_mgr_destroy (TTXProviderMgr *self);


/**
 * Get a provider by its ID
 *
 * @param self a provider mgr
 * @param prov_id a provider id
 *
 * @return a provider or NULL in case of error. Do not
 */
const TTXProvider *ttx_provider_mgr_get_provider (TTXProviderMgr *self,
						  TTXProviderID prov_id);


typedef void (*TTXProviderForeachFunc) (TTXProviderID prov_id,
					const TTXProvider *prov, void *user_data);

/**
 * Execute a function for all providers
 *
 * @param self a provider-mgr
 * @param func function to execute
 * @param user_data user pointer passed to function
 */
void ttx_provider_mgr_foreach (TTXProviderMgr *self, TTXProviderForeachFunc func,
			       void *user_data);




gboolean ttx_provider_mgr_retrieve (TTXProviderMgr *self, TTXProviderID prov_id,
				    unsigned page, unsigned subpage,
				    TTXProviderResultFunc func,
				    gpointer user_data);

#endif /*__TTX_PROVIDER_MGR_H__*/
