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

#include <glib.h>
#include <ttx-provider.h>

struct _TTXProvider {
	char *name;
	char *descr;
	TTXProviderRetrievalFunc retrieval_func;
};

TTXProvider*
ttx_provider_new (const char *name,
		  const char *descr,
		  TTXProviderRetrievalFunc retrieval_func)
{
	TTXProvider *prov;

	g_return_val_if_fail (name, NULL);
	g_return_val_if_fail (descr, NULL);
	g_return_val_if_fail (retrieval_func, NULL);

	prov = g_new0 (TTXProvider, 1);

	prov->name	     = g_strdup (name);
	prov->descr	     = g_strdup (descr);
	prov->retrieval_func = retrieval_func;

	return prov;
}

void
ttx_provider_destroy (TTXProvider *prov)
{
	if (!prov)
		return;

	g_free (prov->name);
	g_free (prov->descr);

	g_free (prov);
}


const char*
ttx_provider_name (const TTXProvider *prov)
{
	g_return_val_if_fail (prov, NULL);

	return prov->name;
}

const char*
ttx_provider_descr (const TTXProvider *prov)
{
	g_return_val_if_fail (prov, NULL);

	return prov->descr;
}

gboolean
ttx_provider_retrieve (const TTXProvider *self,
		       unsigned page, unsigned subpage,
		       const char *path,
		       TTXProviderResultFunc result_func,
		       gpointer user_data)
{
	g_return_val_if_fail (self, FALSE);
	g_return_val_if_fail (page <= 999, FALSE);
	g_return_val_if_fail (subpage > 0, FALSE);
	g_return_val_if_fail (path, FALSE);
	g_return_val_if_fail (result_func, FALSE);

	return self->retrieval_func (page, subpage, path,
				     result_func, user_data);
}
