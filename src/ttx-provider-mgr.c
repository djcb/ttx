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
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "ttx-http.h"
#include "ttx-provider-mgr.h"


struct _TTXProviderMgr {
	GHashTable	*hash;
	gchar		*tmpdir;
};


TTXProviderMgr*
ttx_provider_mgr_new (void)
{
	TTXProviderMgr *self;
	unsigned u;

	self = g_new0 (TTXProviderMgr, 1);

	self->hash = g_hash_table_new_full
		(g_str_hash,
		 g_str_equal,
		 g_free,
		 (GDestroyNotify)ttx_provider_destroy);

	for (u = 0; u != G_N_ELEMENTS(TTX_PROVIDERS); ++u) {

		TTXProvider *prov;
		if (!(prov = TTX_PROVIDERS[u].new_func())) {
			g_warning ("invalid provider %u", u);
			ttx_provider_mgr_destroy (self);
			return NULL;
		}

		g_hash_table_insert (self->hash,
				     g_strdup(TTX_PROVIDERS[u].prov_id),
				     prov);
	}

	self->tmpdir = g_strdup_printf ("%s/ttx-XXXXXX", g_get_tmp_dir());
	self->tmpdir = g_mkdtemp_full (self->tmpdir, 00700);

	return self;
}

void
ttx_provider_mgr_destroy (TTXProviderMgr *self)
{
	if (!self)
		return;

	if (self->tmpdir && access (self->tmpdir, F_OK) == 0)
		if (remove (self->tmpdir) != 0)
			g_warning ("failed to delete %s: %s",
				   self->tmpdir, strerror(errno));

	g_free (self->tmpdir);
	g_hash_table_destroy (self->hash);

	g_free (self);
}


const TTXProvider*
ttx_provider_mgr_get_provider (TTXProviderMgr *self, TTXProviderID prov_id)
{
	TTXProvider *prov;

	g_return_val_if_fail (self, NULL);
	g_return_val_if_fail (prov_id, NULL);

	prov = (TTXProvider*)g_hash_table_lookup (self->hash, prov_id);

	return prov;
}

struct _CBData {
	TTXProviderForeachFunc	func;
	gpointer		user_data;
};
typedef struct _CBData CBData;


static void
each_prov (TTXProviderID prov_id, TTXProvider *prov, CBData *cbdata)
{
	cbdata->func (prov_id, prov, cbdata->user_data);
}

void
ttx_provider_mgr_foreach (TTXProviderMgr *self, TTXProviderForeachFunc func,
			  gpointer user_data)
{
	CBData cbdata;

	g_return_if_fail (self);
	g_return_if_fail (func);

	cbdata.func	 = func;
	cbdata.user_data = user_data;

	g_hash_table_foreach (self->hash, (GHFunc)each_prov, &cbdata);
}



gboolean
ttx_provider_mgr_retrieve (TTXProviderMgr *self, const char *prov_id,
			   unsigned page, unsigned subpage,
			   TTXProviderResultFunc func,
			   gpointer user_data)
{
	const TTXProvider *prov;

	g_return_val_if_fail (self, FALSE);
	g_return_val_if_fail (100 <= page && page <= 999, FALSE);
	g_return_val_if_fail (subpage > 0, FALSE);
	g_return_val_if_fail (prov_id, FALSE);

	prov = ttx_provider_mgr_get_provider (self, prov_id);
	g_return_val_if_fail (prov, FALSE);

	return ttx_provider_retrieve (prov, page, subpage, self->tmpdir,
				      func, user_data);
}
