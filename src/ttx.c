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
#include <libintl.h>
#include <locale.h>

#include <ttx-window.h>

#include <providers/ttx-providers.h>
#include <ttx-provider-mgr.h>

static gboolean
on_key_press_event (GtkWidget *w, GdkEventKey *event_key, TTXWindow *ttx)
{
	switch (event_key->keyval) {
	case GDK_KEY_Q:
	case GDK_KEY_q:
		gtk_main_quit ();
		break;
	default:
		break;
	}

	return FALSE;
}

int
main (int argc, char *argv[])
{
	GtkWidget *win;
	TTXProviderMgr *prov_mgr;

	setlocale (LC_ALL, "");

	bindtextdomain (GETTEXT_PACKAGE, TTXLOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	prov_mgr = ttx_provider_mgr_new ();

	gtk_init (&argc, &argv);

	win = ttx_window_new (prov_mgr);

	g_signal_connect (G_OBJECT(win), "delete-event",
			  G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect (G_OBJECT(win), "key-press-event",
			  G_CALLBACK(on_key_press_event), win);

	gtk_widget_show (win);

	ttx_window_request_page (TTX_WINDOW(win), "nos", 100, 1);

	gtk_main ();

	ttx_provider_mgr_destroy (prov_mgr);

	return 0;
}
