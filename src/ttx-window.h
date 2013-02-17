/*
** Copyright (C) 2013 Dirk-Jan C. Binnema <djcb@djcbsofware.nl>
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


#ifndef __TTX_WINDOW_H__
#define __TTX_WINDOW_H__

#include <gtk/gtk.h>
#include <ttx-provider-mgr.h>

/* other include files */

G_BEGIN_DECLS

/* convenience macros */
#define TTX_TYPE_WINDOW             (ttx_window_get_type())
#define TTX_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),TTX_TYPE_WINDOW,TTXWindow))
#define TTX_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),TTX_TYPE_WINDOW,TTXWindowClass))
#define TTX_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),TTX_TYPE_WINDOW))
#define TTX_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),TTX_TYPE_WINDOW))
#define TTX_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj),TTX_TYPE_WINDOW,TTXWindowClass))

typedef struct _TTXWindow		TTXWindow;
typedef struct _TTXWindowClass		TTXWindowClass;
typedef struct _TTXWindowPrivate        TTXWindowPrivate;

struct _TTXWindow {
	 GtkWindow parent;
	/* insert public members, if any */

	/* private */
	TTXWindowPrivate *priv;
};


struct _TTXWindowClass {
	GtkWindowClass parent_class;
};


/**
 * Get the GType for a TTXWindow
 *
 * @return the GType
 */
GType        ttx_window_get_type    (void) G_GNUC_CONST;

/**
 * create a new TTXWindow instance
 *
 * @param prov_mgr a provider manager
 *
 * @return a new TTXWindow, or NULL in case of error
 */
GtkWidget *ttx_window_new  (TTXProviderMgr *prov_mgr) G_GNUC_WARN_UNUSED_RESULT;

/**
 * request the TTXWindow to show a certain page
 *
 * @param self a TTXWindow
 * @param prov_id the provider-id for the page to request
 * @param page the page number [100 .. 999]
 * @param subpage the subpage number [1 .. 99]
 */
void ttx_window_request_page (TTXWindow *self, TTXProviderID prov_id,
			      unsigned page, unsigned subpage);

/**
 * set the image shown in the ttxwindow to some file
 *
 * @param self a TTXWindow
 * @param path path to some image file
 *
 * @return TRUE if it worked, FALSE in case of error
 */
gboolean ttx_window_set_image (TTXWindow *self, const char *path);


G_END_DECLS

#endif /* __TTX_WINDOW_H__ */
