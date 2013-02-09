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


typedef enum {
	TTX_PROVIDER_NOS_TELETEKST = 0,
	TTX_PROVIDER_EEN_BE,

	TTX_PROVIDER_NUM
} TTXProviderID;


typedef enum {
	TTX_RETRIEVAL_OK,
	TTX_RETRIEVAL_ERROR
} TTXRetrievalStatus;

typedef void (*TTXProviderResultFunc) (TTXRetrievalStatus status,
				       TTXProviderID prov_id,
				       unsigned page,
				       unsigned subpage,
				       const char *path,
				       GSList *links,
				       gpointer user_data);


typedef void (*TTXProviderRetrievalFunc) (unsigned page, unsigned subpage,
					  const char *path,
					  TTXProviderResultFunc func,
					  gpointer user_data);

/**
 * Retrieve a page (image) for some provider
 *
 * @param self a TTXProviderMgr
 * @param prov_id a provider-id
 * @param page a page number
 * @param subpage a subpage number
 * @param func callback function to call with results
 * @param user_data user pointer passed to callback
 */
void ttx_provider_mgr_retrieve (TTXProviderMgr *self, TTXProviderID prov_id,
				unsigned page, unsigned subpage,
				TTXProviderResultFunc func,
				gpointer user_data);


#endif /*__TTX_PROVIDER_MGR_H__*/
