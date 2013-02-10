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

#ifndef __TTX_PROVIDER_H__
#define __TTX_PROVIDER_H__

#include <glib.h>

typedef enum {
	TTX_RETRIEVAL_OK,
	TTX_RETRIEVAL_ERROR
} TTXRetrievalStatus;


typedef void (*TTXProviderResultFunc) (TTXRetrievalStatus status,
				       unsigned page,
				       unsigned subpage,
				       const char *path,
				       GSList *links,
				       gpointer user_data);


typedef gboolean (*TTXProviderRetrievalFunc) (unsigned page, unsigned subpage,
					      const char *path,
					      TTXProviderResultFunc func,
					      gpointer user_data);

struct		_TTXProvider;
typedef struct	_TTXProvider TTXProvider;

/**
 * Create new TTXProvider instance
 *
 * @param prov_id the provider-id
 * @param name name of the provider
 * @param descr description of the provider
 * @param retrieval_func a function to retrieve page images for this provider
 *
 * @return a new provider, or NULL in case of error. Free with ttx_provider_destroy
 */
TTXProvider* ttx_provider_new (const char *name,
			       const char *descr,
			       TTXProviderRetrievalFunc retrieval_func);

/** Destroy a TTXProvider
 *
 * @param self a provider instance or NULL
 */
void ttx_provider_destroy (TTXProvider *self);


/**
 * Get the name of the provider
 *
 * @param self a provider
 *
 * @return the name of the provider or NULL in case of error
 */
const char* ttx_provider_name (const TTXProvider *self);

/**
 * Get the description of the provider
 *
 * @param self a provider
 *
 * @return the description of the provider or NULL in case of error
 */
const char* ttx_provider_descr (const TTXProvider *self);


/**
 * Asynchronously retrieve a teletext page
 *
 * @param self a provider
 * @param page the page number ([100...999])
 * @param subpage the subpage number ([1..99])
 * @param path the path to store the result
 * @param result_func the function to call when retrieval is complete or has failed
 * @param user_data user pointer passed to result_func
 *
 * @return TRUE if starting retrieval worked, FALSE otherwise
 */
gboolean ttx_provider_retrieve (const TTXProvider *self,
				unsigned page, unsigned subpage,
				const char *path,
				TTXProviderResultFunc result_func,
				gpointer user_data);


#endif /*__TTX_PROVIDER_H__*/
