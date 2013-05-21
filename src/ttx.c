/*
** Copyright) 2013 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <ttx-window.h>

#include <providers/ttx-providers.h>
#include <ttx-provider-mgr.h>

struct _Opts {
	gboolean	  debug;
	gboolean	  version;
	unsigned	  page;
	unsigned	  subpage;
	char		 *prov_id;
	char		**params;
};
typedef struct _Opts Opts;

static void
free_opts (Opts *opts)
{
	g_free (opts->prov_id);
	g_strfreev (opts->params);
}

static void
quit (GtkWidget *w)
{
	/* explicitly destroy the window, so it
	 * can cleanup tmp files etc. */
	gtk_widget_destroy (w);
	gtk_main_quit ();
}


static gboolean
on_key_press_event (GtkWidget *w, GdkEventKey *event_key, TTXWindow *ttx)
{
	switch (event_key->keyval) {
	case GDK_KEY_Q:
	case GDK_KEY_q:
		quit (GTK_WIDGET(ttx));
		break;
	default:
		break;
	}
	return FALSE;
}


static gboolean
setup_gui (TTXProviderMgr *prov_mgr, Opts *opts)
{
	GtkWidget *win;
	GSettings *settings;

	settings = g_settings_new (TTX_APP_ID);
	if (!settings) {
		g_warning ("settings not found; please install them");
		return FALSE;
	}
	g_object_unref (settings); /* just testing...*/

	win = ttx_window_new (prov_mgr);

	g_signal_connect (G_OBJECT(win), "delete-event",
			  G_CALLBACK(quit), win);

	g_signal_connect (G_OBJECT(win), "key-press-event",
			  G_CALLBACK(on_key_press_event), win);

	gtk_widget_show (win);

	ttx_window_request_page (TTX_WINDOW(win),
				 opts->prov_id,	/* NULL means 'last one' */
				 opts->page    ? opts->page : 100,
				 opts->subpage ? opts->subpage : 1);
	return TRUE;
}



static GOptionGroup*
get_option_group (Opts *opts)
{
        GOptionGroup *og;
        GOptionEntry entries[] = {
                {"debug", 'd', 0, G_OPTION_ARG_NONE, &opts->debug,
                 N_("enable debugging (false)"), NULL},
		{"version", 'V', 0, G_OPTION_ARG_NONE, &opts->version,
		 N_("show version and exit"), NULL},
		{G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY,
                 &opts->params, "parameters", NULL},
                {NULL, 0, 0, 0, NULL, NULL, NULL}
        };
        memset (opts, 0, sizeof(Opts));

        og = g_option_group_new("ttx", _("options for ttx"),
                                "", NULL, NULL);
        g_option_group_add_entries(og, entries);

        return og;
}

static gboolean
process_options (Opts *opts, int *argcp, char ***argvp, GError **err)
{
        gboolean rv;
        GOptionContext *octx;

        octx = g_option_context_new ("- ttx options");

        g_option_context_set_main_group (octx, get_option_group(opts));
        rv = g_option_context_parse (octx, argcp, argvp, err);
        g_option_context_free (octx);

        return rv;
}

static gboolean
check_address (TTXProviderMgr *prov_mgr, const char *prov_id,
	       unsigned page, unsigned subpage)
{
	const TTXProvider *prov;

	prov = ttx_provider_mgr_get_provider (prov_mgr, prov_id);
	if (!prov) {
		g_printerr (_("unknown teletext provider %s\n"),
			    prov_id);
		return FALSE;
	}

	if (page < 100 || page > 999) {
		g_printerr (_("page must be in the range [100..999]\n"));
		return FALSE;
	}

	if (subpage < 1 || subpage > 99) {
		g_printerr (_("subpage must be in the range [1..99]\n"));
		return FALSE;
	}

	return TRUE;
}


static gboolean
get_address_maybe (TTXProviderMgr *prov_mgr, Opts *opts, GError **err)
{
	GRegex *rx;
	GMatchInfo *minfo;
	char* str;
	const char *addr;

	minfo = NULL;
	str   = NULL;

	if (!opts->params || !opts->params[0])
		return TRUE; /* nothing to do here */

	addr = opts->params[0];

	rx = g_regex_new ("(\\w+):(\\d{3})(/(\\d{1,2}))?", 0, 0, err);
	if (!rx)
		return FALSE;

	if (!g_regex_match (rx, addr, 0, &minfo)) {
		g_printerr (_("invalid link; expected: "
			      "<provider-id>:<page>[/<subpage>]\n"
			      "e.g., nos:101/1 or yle:200\n"));
		g_match_info_free (minfo);
		g_regex_unref (rx);
		return FALSE;
	}

	opts->prov_id = g_match_info_fetch (minfo, 1);
	str = g_match_info_fetch (minfo, 2);

	opts->page = atoi (str);
	g_free (str);

	if (g_match_info_get_match_count (minfo) < 5)
		opts->subpage = 1;
	else {
		str = g_match_info_fetch (minfo, 4);
		opts->subpage = atoi (str);
		g_free (str);
	}

	g_match_info_free (minfo);
	g_regex_unref (rx);

	return check_address (prov_mgr, opts->prov_id, opts->page,
			      opts->subpage);
}

static int
version (void)
{
	const char *blurb =
		_("ttx teletext browser, version " PACKAGE_VERSION "\n"
		  "Copyright (C) 2013 Dirk-Jan C. Binnema\n"
		  "License GPLv3+: GNU GPL version 3 or later "
		  "<http://gnu.org/licenses/gpl.html>.\n"
		  "This is free software: you are free to change "
		  "and redistribute it.\n"
		  "There is NO WARRANTY, to the extent permitted by law.");

	g_print ("%s\n", blurb);

	return 0;
}

static gboolean
setup_l10n (void)
{
	errno = 0;

	if (!setlocale (LC_ALL, ""))
		goto errexit;

	if (!bindtextdomain (GETTEXT_PACKAGE, TTX_LOCALEDIR))
		goto errexit;

	if (!bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8"))
		goto errexit;

	if (!textdomain (GETTEXT_PACKAGE))
		goto errexit;

	return TRUE;

errexit:
	g_printerr ("failed to setup l10n: %s", strerror (errno));
	return FALSE;
}

int
main (int argc, char *argv[])
{
	Opts opts;
	GError *err;
	TTXProviderMgr *prov_mgr;
	int rv;

	if (!setup_l10n ())
		return 1;

	rv	 = 1;
	err	 = NULL;

	prov_mgr = ttx_provider_mgr_new ();

	memset (&opts, 0, sizeof(Opts));

	if (!process_options (&opts, &argc, &argv, &err))
		goto leave;

	if (opts.version)
		return version ();

	if(!get_address_maybe (prov_mgr, &opts, &err))
		goto leave;

	gtk_init (&argc, &argv);

	if (!setup_gui (prov_mgr, &opts))
		goto leave;

	gtk_main ();
	rv = 0;

leave:
	free_opts (&opts);

	ttx_provider_mgr_destroy (prov_mgr);
	if (err) {
		g_printerr ("%s\n", err ? err->message :
			    _("an error occurred"));
		g_clear_error (&err);
	}

	return rv;
}
