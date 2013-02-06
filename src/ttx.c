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

#include <ttx-window.h>

struct _TTXData {

};
typedef struct _TTXData TTXData;

static TTXData*
ttx_data_new (void)
{
	TTXData *self;

	self	   = g_new0 (TTXData, 1);
	return self;
}

static void
ttx_data_destroy (TTXData *data)
{
	if (!data)
		return;

	g_free (data);
}


int
main (int argc, char *argv[])
{
	GtkWidget *win;
	TTXData *data;

	data = ttx_data_new ();

	gtk_init (&argc, &argv);

	win = ttx_window_new ();
	g_signal_connect (G_OBJECT(win), "delete-event",
			  G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show (win);
	ttx_window_request_page (TTX_WINDOW(win), 100, 1);

	gtk_main ();

	ttx_data_destroy (data);

	return 0;
}
