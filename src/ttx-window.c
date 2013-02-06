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

#define GETTEXT_PACKAGE "ttx"
#include <glib/gi18n-lib.h>

#include <stdlib.h>

#include "ttx-window.h"
#include "ttx-link.h"
#include "ttx-provider-mgr.h"


/* 'private'/'protected' functions */
static void ttx_window_class_init (TTXWindowClass *klass);
static void ttx_window_init       (TTXWindow *obj);
static void ttx_window_finalize   (GObject *obj);

/* list my signals  */
enum {
	/* PAGE_REQUESTED, */

	LAST_SIGNAL
};


struct _TTXWindowPrivate {

	GtkWidget *image;
	GtkWidget *toolbar;
	GtkWidget *page_entry, *subpage_entry;

	TTXProviderID provider_id;
	unsigned page, subpage;
	GSList *links;

	TTXProviderMgr *prov_mgr;

	gboolean special_cursor;
};
#define TTX_WINDOW_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                        TTX_TYPE_WINDOW, \
                                        TTXWindowPrivate))
/* globals */
static GtkWindowClass	*parent_class = NULL;
/* static guint signals[LAST_SIGNAL]     = {0}; */

G_DEFINE_TYPE (TTXWindow, ttx_window, GTK_TYPE_WINDOW);

static void
ttx_window_class_init (TTXWindowClass *klass)
{
	GObjectClass *gobject_class;
	gobject_class = (GObjectClass*) klass;

	parent_class            = g_type_class_peek_parent (klass);
	gobject_class->finalize = ttx_window_finalize;

	g_type_class_add_private (gobject_class, sizeof(TTXWindowPrivate));
}

static void
free_links (TTXWindow *self)
{
	g_slist_foreach (self->priv->links, (GFunc)ttx_link_destroy, NULL);
	g_slist_free (self->priv->links);
	self->priv->links = NULL;
}


static void
add_links (TTXWindow *self, GSList *links)
{
	free_links (self);
	self->priv->links = links;
}


static void
update_entry (TTXWindow *self)
{
	char digits[4];
	sprintf (digits,"%u", self->priv->page);
	gtk_entry_set_text (GTK_ENTRY(self->priv->page_entry), digits);
	sprintf (digits,"%u", self->priv->subpage);
	gtk_entry_set_text (GTK_ENTRY(self->priv->subpage_entry), digits);
}


static void
on_completed (TTXRetrievalStatus status, TTXProviderID prov_id,
	      unsigned page, unsigned subpage, const char *path,
	      GSList *links, TTXWindow *self)
{
	if (status != TTX_RETRIEVAL_OK) {
		g_warning ("an error occured retrieving %u/%u",
			   page, subpage);
		return;
	}

	gtk_image_set_from_file (GTK_IMAGE(self->priv->image), path);
	add_links (self, links);

	self->priv->page    = page;
	self->priv->subpage = subpage;

	update_entry (self);
}


static void
on_entry_activate (GtkEntry *entry, TTXWindow *self)
{
	const char *pagestr, *subpagestr;
	unsigned page, subpage;

	pagestr	   = gtk_entry_get_text
		(GTK_ENTRY(self->priv->page_entry));
	subpagestr = gtk_entry_get_text
		(GTK_ENTRY(self->priv->subpage_entry));

	page	= atoi (pagestr);
	subpage = atoi (subpagestr);

	ttx_window_request_page (self, page, subpage);
}


static GtkWidget*
get_entry_box (TTXWindow *self)
{
	GtkWidget *entry_box, *entry_main, *entry_sub;

	entry_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);

	entry_main = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY(entry_main), 3);
	gtk_entry_set_width_chars (GTK_ENTRY(entry_main), 3);

	entry_sub = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY(entry_sub), 2);
	gtk_entry_set_width_chars (GTK_ENTRY(entry_sub), 2);

	gtk_box_pack_start (GTK_BOX(entry_box), entry_main, FALSE, FALSE, 2);
	gtk_box_pack_start (GTK_BOX(entry_box), entry_sub, FALSE, FALSE, 2);

	self->priv->page_entry    = entry_main;
	self->priv->subpage_entry = entry_sub;

	g_signal_connect (entry_main, "activate", G_CALLBACK(on_entry_activate),
			  self);
	g_signal_connect (entry_sub, "activate", G_CALLBACK(on_entry_activate),
			  self);

	update_entry (self);

	return entry_box;
}



enum Buttons {
	BUTTON_PREV		= 0,
	BUTTON_NEXT,
	BUTTON_REFRESH,
	BUTTON_NUM
};

#define BUTTON_ID "button-id"

static void
on_clicked (GtkToolButton *b, TTXWindow *self)
{
	unsigned u;
	unsigned page, subpage;

	u = GPOINTER_TO_SIZE(g_object_get_data (G_OBJECT(b), BUTTON_ID));

	switch (u) {
	case BUTTON_PREV:
		page	= self->priv->page - 1;
		subpage = 1;
		break;
	case BUTTON_NEXT:
		page	= self->priv->page + 1;
		subpage = 1;
		break;
	case BUTTON_REFRESH:
		page	= self->priv->page;
		subpage = self->priv->subpage;
		break;
	default:
		g_return_if_reached ();
		return;
	};

	if (page >= 100 && page <= 999)
		ttx_window_request_page (self, page, subpage);
}



static GtkWidget*
get_toolbar (TTXWindow *self)
{
	GtkWidget *toolbox;
	GtkToolItem *btn;

	toolbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	btn = gtk_tool_button_new_from_stock (GTK_STOCK_GO_BACK);
	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_PREV));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (toolbox), get_entry_box(self),
			    FALSE, FALSE, 0);

	btn = gtk_tool_button_new_from_stock (GTK_STOCK_GO_FORWARD);
	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_NEXT));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	btn = gtk_tool_button_new_from_stock (GTK_STOCK_REFRESH);
	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_REFRESH));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	return toolbox;
}

static gboolean
cursor_on_link (TTXWindow *self, GtkWidget *w, GdkEvent *event,
	 unsigned *page, unsigned *subpage)
{
	GSList *cur;
	GdkWindow *win;
	GdkDevice *dev;
	guint x, y;

	dev = gdk_event_get_device (event);
	win = gtk_widget_get_window (w);

	gdk_window_get_device_position (win, dev, (int*)&x, (int*)&y, NULL);

	for (cur = self->priv->links; cur; cur = g_slist_next (cur)) {

		TTXLink *link;
		link = (TTXLink*)cur->data;

		if (link->left <= x && x <= link->right &&
		    link->top  <= y && y <= link-> bottom) {
			*page	 = link->page;
			*subpage = link->subpage;
			/* g_print ("!!\n"); */
			return TRUE;
		}
	}

	return FALSE;
}


static gboolean
on_button_press_event (GtkWidget *w, GdkEvent *event, TTXWindow *self)
{
	unsigned page, subpage;

	if (cursor_on_link (self, w, event, &page, &subpage))
		ttx_window_request_page (self, page, subpage);

	return TRUE;
}


static gboolean
on_motion_notify_event (GtkWidget *w, GdkEvent *event, TTXWindow *self)
{
	GdkWindow *win;
	unsigned page, subpage;
	GdkCursor *cursor;

	win = gtk_widget_get_window (w);

	if (!cursor_on_link (self, w, event, &page, &subpage))
		cursor = NULL;
	else
		cursor = gdk_cursor_new_for_display (
			gdk_display_get_default(),
			GDK_HAND1);

	/* small optimization: only reset the cursor if needed */
	if (cursor || self->priv->special_cursor) {
		gdk_window_set_cursor (win, cursor);
		self->priv->special_cursor = cursor ? TRUE : FALSE;
	}

	return TRUE;
}

static GtkWidget *
get_image_box (TTXWindow *self, GtkWidget *img)
{
	GtkWidget *ebox;

	ebox = gtk_event_box_new ();

	gtk_widget_add_events (ebox,
			       GDK_POINTER_MOTION_MASK|
			       GDK_LEAVE_NOTIFY_MASK);

	g_signal_connect (G_OBJECT(ebox), "button-press-event",
			  G_CALLBACK(on_button_press_event), self);
	g_signal_connect (G_OBJECT(ebox), "motion-notify-event",
			  G_CALLBACK(on_motion_notify_event), self);

	gtk_container_add (GTK_CONTAINER(ebox), img);

	return ebox;
}


static GtkWidget*
get_image (TTXWindow *self)
{
	GtkWidget *img;

	img = gtk_image_new ();
	gtk_widget_set_size_request (img, 440, 336);

	return img;
}


static void
ttx_window_init (TTXWindow *self)
{
	GtkWidget *vbox, *ebox;

	self->priv = TTX_WINDOW_GET_PRIVATE(self);

	self->priv->prov_mgr	   = ttx_provider_mgr_new ();
	self->priv->provider_id	   = TTX_PROVIDER_NOS_TELETEKST;
	self->priv->page	   = 100;
	self->priv->subpage	   = 1;
	self->priv->special_cursor = FALSE;

	gtk_window_set_title (GTK_WINDOW(self), _("TTX Teletext Browser"));
	gtk_window_set_resizable (GTK_WINDOW(self), FALSE);

	gtk_window_set_icon_name (GTK_WINDOW(self), "ttx");
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	self->priv->toolbar = get_toolbar (self);
	self->priv->image   = get_image (self);

	ebox = get_image_box (self, self->priv->image);

	gtk_box_pack_start (GTK_BOX(vbox), ebox,TRUE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX(vbox), self->priv->toolbar,
			    FALSE, FALSE, 0);
	gtk_widget_show_all (vbox);

	gtk_container_add (GTK_CONTAINER(self), vbox);
}

static void
ttx_window_finalize (GObject *obj)
{
	TTXWindow *self;

	self = (TTXWindow*)obj;

	ttx_provider_mgr_destroy (self->priv->prov_mgr);
	free_links (self);

	G_OBJECT_CLASS(parent_class)->finalize (obj);
}

GtkWidget*
ttx_window_new (void)
{
	return GTK_WIDGET(g_object_new(TTX_TYPE_WINDOW, NULL));
}

gboolean
ttx_window_set_image (TTXWindow *self, const char *path)
{
	g_return_val_if_fail (TTX_IS_WINDOW(self), FALSE);
	g_return_val_if_fail (path, FALSE);

	gtk_image_set_from_file (GTK_IMAGE(self->priv->image), path);

	return TRUE;
}


void
ttx_window_request_page (TTXWindow *self, unsigned page, unsigned subpage)
{
	g_return_if_fail (TTX_IS_WINDOW(self));
	g_return_if_fail (page >= 100 && page <= 999);
	g_return_if_fail (subpage > 0);

	if (page > 999 || subpage < 1) {
		g_warning ("invalid page %u/%u", page, subpage);
		update_entry (self);
		return;
	}

	ttx_provider_mgr_retrieve
		(self->priv->prov_mgr,
		 self->priv->provider_id,
		 page, subpage,
		 (TTXProviderResultFunc)on_completed,
		 self);
}
