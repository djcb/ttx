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

#include "ttx-http.h"
#include "ttx-provider-mgr.h"

#include "ttx-provider-nos-teletekst.h"
#include "ttx-provider-een-be.h"

static void register_providers (TTXProviderMgr *self);

struct _Prov {
	unsigned			 prov_id;
	char				*name;
	char				*descr;
	TTXProviderRetrievalFunc	 retrieval_func;
};

typedef struct _Prov Prov;

static Prov*
prov_new (TTXProviderID prov_id, const char *name,
	  const char *descr, TTXProviderRetrievalFunc uri_func)
{
	Prov *prov;

	prov		     = g_new0 (Prov, 1);
	prov->prov_id	     = prov_id;
	prov->name	     = g_strdup (name);
	prov->descr	     = g_strdup (descr);
	prov->retrieval_func = uri_func;

	return prov;
}


static void
prov_destroy (Prov *prov)
{
	if (!prov)
		return;

	g_free (prov->name);
	g_free (prov->descr);

	g_free (prov);
}



struct _TTXProviderMgr {
	char		*tmpdir;
	GHashTable	*prov_hash;
};


TTXProviderMgr *
ttx_provider_mgr_new (void)
{
	TTXProviderMgr *self;

	self = g_new0 (TTXProviderMgr, 1);

	self->tmpdir = g_strdup_printf ("%s/ttx-XXXXXX",
					g_get_tmp_dir());
	self->tmpdir = g_mkdtemp_full (self->tmpdir, 00700);

	self->prov_hash =
		g_hash_table_new_full (g_direct_hash,
				       g_direct_equal,
				       NULL,
				       (GDestroyNotify)prov_destroy);
	register_providers (self);

	return self;
}


void
ttx_provider_mgr_destroy (TTXProviderMgr *self)
{
	if (!self)
		return;

	g_free (self->tmpdir);

	if (self->prov_hash)
		g_hash_table_destroy (self->prov_hash);

	g_free (self);
}




/**
 * Register a new TTXProvider
 *
 * @param self a TTXProviderMgr
 * @param prov_id a provider-id
 * @param name name for the provider
 * @param descr description for the provider
 * @param url_func function that returns the URL for a given page/subpage pair
 */
static void
register_provider (TTXProviderMgr *self,
		   TTXProviderID prov_id, const char *name,
		   const char *descr, TTXProviderRetrievalFunc url_func)
{
	Prov *prov;

	g_return_if_fail (self);
	g_return_if_fail (name);
	g_return_if_fail (descr);
	g_return_if_fail (url_func);

	prov = prov_new (prov_id, name, descr, url_func);
	g_hash_table_insert (self->prov_hash,
			     GSIZE_TO_POINTER(prov_id), prov);
}

void
ttx_provider_mgr_retrieve (TTXProviderMgr *self, TTXProviderID prov_id,
			   unsigned page, unsigned subpage,
			   TTXProviderResultFunc func,
			   gpointer user_data)
{
	Prov *prov;

	g_return_if_fail (self);
	g_return_if_fail (100 <= page && page <= 999);
	g_return_if_fail (subpage > 0);

	prov = g_hash_table_lookup (self->prov_hash,
				    GSIZE_TO_POINTER(prov_id));
	if (!prov) {
		g_warning ("unknown provider %u", prov_id);
		return;
	}

	prov->retrieval_func (page, subpage, self->tmpdir,
			      func, user_data);
}


static void
register_providers (TTXProviderMgr *self)
{
	register_provider (self, TTX_PROVIDER_NOS_TELETEKST,
			   "NOS Teletekst",
			   _("Dutch Teletekst Service"),
			   ttx_provider_nos_teletekst_retrieve);


	register_provider (self, TTX_PROVIDER_EEN_BE,
			   "Een BE Teletekst",
			   _("Flemish Teletekst Service"),
			   ttx_provider_een_be_retrieve);

	/* add other provider ... */
}
