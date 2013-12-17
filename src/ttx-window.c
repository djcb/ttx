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

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "ttx-window.h"
#include "ttx-link.h"

static void	ttx_window_class_init  (TTXWindowClass *klass);
static void	ttx_window_init        (TTXWindow *obj);
static void	ttx_window_finalize    (GObject *obj);
static void     ttx_window_constructed (GObject *obj);

/* signals */
typedef enum {
	/* PAGE_REQUESTED, */
	SIGNAL_NUM
} TTXWindowSignals;


/* properties */
enum {
	PROP_0            = 0, /**< dummy */
	PROP_PROVIDER_MGR,     /**< TTXProviderMgr */
	PROP_NUM
} TTXWindowProperties;


struct _TTXWindowPrivate {

	GtkWidget	*image;
	GtkWidget	*toolbar;
	GtkWidget	*page_entry, *subpage_entry;
	GtkWidget       *combo;

	unsigned	page, subpage;

	GSettings       *settings;
	GSList		*links;

	TTXProviderMgr  *prov_mgr;
	gboolean	on_link;

	char	        *img_file;

	gboolean	 retrieving;
	char		*prov_id;
};


static const char*
get_provider_id (TTXWindow *self)
{
	g_free (self->priv->prov_id);

	return self->priv->prov_id =
		g_settings_get_string (self->priv->settings,
				       "provider-id");
}

static void
set_provider_id (TTXWindow *self, const char *prov_id)
{
	g_settings_set_string (self->priv->settings,
			       "provider-id", prov_id);
}


#define TTX_WINDOW_GET_PRIVATE(o)      (G_TYPE_INSTANCE_GET_PRIVATE((o), \
                                        TTX_TYPE_WINDOW, \
                                        TTXWindowPrivate))
/* globals */
static GtkWindowClass	*parent_class = NULL;
/* static guint signals[SIGNAL_NUM]     = {0}; */


static GParamSpec *obj_properties[PROP_NUM] = { NULL, };

static void
ttx_window_set_property (GObject *obj, guint prop_id,
			 const GValue *val, GParamSpec *pspec)
{
	TTXWindow *self;

	self = TTX_WINDOW(obj);

	switch (prop_id) {
	case PROP_PROVIDER_MGR:
		self->priv->prov_mgr = g_value_get_pointer (val);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, prop_id, pspec);
		break;
	}
}

static void
ttx_window_get_property (GObject *obj, guint prop_id,
	      GValue *val, GParamSpec *pspec)
{
	TTXWindow *self;

	self = TTX_WINDOW(obj);

	switch (prop_id)  {
	case PROP_PROVIDER_MGR:
		g_value_set_pointer (val, self->priv->prov_mgr);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (self, prop_id, pspec);
		break;
	}
}


G_DEFINE_TYPE (TTXWindow, ttx_window, GTK_TYPE_WINDOW);

static void
ttx_window_class_init (TTXWindowClass *klass)
{
	GObjectClass *gobj_class;

	gobj_class = (GObjectClass*) klass;

	parent_class            = g_type_class_peek_parent (klass);

	gobj_class->finalize	 = ttx_window_finalize;
	gobj_class->set_property = ttx_window_set_property;
	gobj_class->get_property = ttx_window_get_property;
	gobj_class->constructed  = ttx_window_constructed;

	g_type_class_add_private (gobj_class, sizeof(TTXWindowPrivate));

	obj_properties[PROP_PROVIDER_MGR] =
		g_param_spec_pointer ("provider-mgr",
				      "provider-mgr",
				      "TTXProviderMgr construct property",
				      G_PARAM_CONSTRUCT_ONLY|G_PARAM_WRITABLE);

	g_object_class_install_properties (gobj_class,
					   PROP_NUM, obj_properties);
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


/* clear the old image file, and set it to path (which may be NULL) */
static void
reset_img_file (TTXWindow *self, const char *path)
{
	/* /\* remove the _old_ tempfile *\/ */
	if (self->priv->img_file &&
	    access (self->priv->img_file, F_OK) == 0) {
		if (remove (self->priv->img_file) != 0)
			g_warning ("failed to unlink %s: %s",
				   path, strerror (errno));
	}

	g_free (self->priv->img_file);
	self->priv->img_file = path ? g_strdup (path) : NULL;
}


static void
on_completed (TTXRetrievalStatus status,
	      unsigned page, unsigned subpage, const char *path,
	      GSList *links, TTXWindow *self)
{
	GdkPixbuf *pixbuf;
	int h, w;

	self->priv->retrieving = FALSE;
	
	if (status != TTX_RETRIEVAL_OK) {
		g_warning ("an error occured retrieving %u/%u",
			   page, subpage);
		update_entry (self);
		return;
	}

	gtk_image_set_from_file (GTK_IMAGE(self->priv->image), path);

	/* resize the window */
	pixbuf = gtk_image_get_pixbuf (GTK_IMAGE(self->priv->image));
	if (pixbuf) {
		h = gdk_pixbuf_get_height (pixbuf);
		w = gdk_pixbuf_get_width (pixbuf);
		gtk_widget_set_size_request (self->priv->image, w, h);
	}

	reset_img_file (self, path);

	add_links (self, links);

	self->priv->page = page;
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

	ttx_window_request_page (self, NULL,
				 page, subpage);
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
		ttx_window_request_page (self, NULL, page, subpage);
}

typedef enum {
	COL_PROVIDER_ID,
	COL_PROVIDER_NAME,
	COL_NUM
} TTXColumns;

static void
on_combo_changed (GtkComboBox *combo, TTXWindow *self)
{
	const char *prov_id;

	/* update page if we weren't already retrieving;
	 * this distinguishes between user changing combo
	 * and ttx_window_request_page */
	if (self->priv->retrieving)
		return;

	prov_id = gtk_combo_box_get_active_id (combo);
	set_provider_id (self, prov_id);

	ttx_window_request_page (self, prov_id, 100, 1);
}

static void
each_prov (TTXProviderID id, const TTXProvider *prov, GtkListStore *store)
{
	GtkTreeIter iter;
	const char *name;

	name = ttx_provider_name (prov);

	gtk_list_store_append (store, &iter);
	gtk_list_store_set (store, &iter,
			    COL_PROVIDER_ID, id,
			    COL_PROVIDER_NAME, name,
			    -1);
}

static GtkWidget*
get_provider_combo (TTXWindow *self)
{
	GtkWidget *combo;
	GtkListStore *model;
	GtkCellRenderer *render;

	model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

	ttx_provider_mgr_foreach (self->priv->prov_mgr,
				  (TTXProviderForeachFunc)each_prov,
				  model);

	combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL(model));
	g_object_unref (model);

	gtk_combo_box_set_id_column (GTK_COMBO_BOX(combo), COL_PROVIDER_ID);
	render = gtk_cell_renderer_text_new ();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), render, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), render,
				       "text", COL_PROVIDER_NAME, NULL );

	g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(on_combo_changed),
			  self);
	return combo;
}


static GtkToolItem*
tool_button (const char *name)
{
	GtkWidget *icon;

	icon = gtk_image_new_from_icon_name
		(name, GTK_ICON_SIZE_SMALL_TOOLBAR);
	
	return gtk_menu_tool_button_new (icon, NULL);
}


static GtkWidget*
get_toolbar (TTXWindow *self)
{
	GtkWidget	*toolbox;
	GtkToolItem	*btn;
	
	toolbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	btn = tool_button ("go-previous");

	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_PREV));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_widget_set_tooltip_text (GTK_WIDGET(btn), _("Previous page"));
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (toolbox), get_entry_box(self),
			    FALSE, FALSE, 0);

	btn = tool_button ("go-next");
	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_NEXT));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_widget_set_tooltip_text (GTK_WIDGET(btn), _("Next page"));
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	btn = tool_button ("view-refresh");
	g_object_set_data (G_OBJECT(btn), BUTTON_ID,
			   GSIZE_TO_POINTER(BUTTON_REFRESH));
	g_signal_connect (btn, "clicked", G_CALLBACK(on_clicked), self);
	gtk_widget_set_tooltip_text (GTK_WIDGET(btn), _("Refresh page"));
	gtk_box_pack_start (GTK_BOX (toolbox), GTK_WIDGET(btn),
			    FALSE, FALSE, 0);

	self->priv->combo = get_provider_combo (self);
	gtk_box_pack_end (GTK_BOX(toolbox), self->priv->combo,
			  FALSE, FALSE, 0);
	return toolbox;
}

static gboolean
pointer_on_link (TTXWindow *self, GtkWidget *w, GdkEvent *event,
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
			return TRUE;
		}
	}

	return FALSE;
}


static gboolean
on_button_press_event (GtkWidget *w, GdkEvent *event, TTXWindow *self)
{
	unsigned page, subpage;
	const char *prov_id;

	prov_id = gtk_combo_box_get_active_id (
		GTK_COMBO_BOX(self->priv->combo));

	if (pointer_on_link (self, w, event, &page, &subpage))
		ttx_window_request_page (self, prov_id, page, subpage);

	return TRUE;
}


static gboolean
on_motion_notify_event (GtkWidget *w, GdkEvent *event, TTXWindow *self)
{
	unsigned page, subpage;
	GdkCursor *cursor;
	gboolean on_link;

	on_link = pointer_on_link (self, w, event, &page, &subpage);
	if (on_link == self->priv->on_link)
		return TRUE; /* already in the right state */

	if (!on_link)
		cursor = NULL; /* default cursor */
	else
		cursor = gdk_cursor_new_for_display (
			gdk_display_get_default(),
			GDK_HAND1);

	gdk_window_set_cursor (gtk_widget_get_window(w), cursor);
	self->priv->on_link = on_link;

	return TRUE;
}

static void
on_drag_data_get (GtkWidget *widget, GdkDragContext *drag_context,
		  GtkSelectionData *sel_data, guint info, guint time,
		  TTXWindow *self)
{
	char	*uris[2] = { NULL, NULL };
	GError	*err;

	if (!self->priv->img_file)
		return;

	err = NULL;
	uris[0] = g_filename_to_uri (self->priv->img_file, NULL, &err);
	if (!uris[0]) {
		g_warning ("fail to get an uri for %s: %s",
			   self->priv->img_file,
			   err ? err->message : "something went wrong");
		g_clear_error (&err);
		return;
	}

	gtk_selection_data_set_uris (sel_data, uris);
	g_free (uris[0]);
}

static void
setup_dnd (TTXWindow *self, GtkWidget *ebox)
{
	GtkTargetEntry entries[] = {
		{ "text/uri-list", 0, 1 },
		{ "application/x-rootwindow-drop", 0, 2 }
	};


	/* note: without this, D&D does not work; see the Gtk docs */
	gtk_event_box_set_visible_window (GTK_EVENT_BOX(ebox),
					  FALSE);

	/* setup drag & drop */
	gtk_drag_source_set (ebox, GDK_BUTTON1_MASK,
			     entries, G_N_ELEMENTS(entries),
			     GDK_ACTION_COPY);
	g_signal_connect (G_OBJECT(ebox), "drag-data-get",
			  G_CALLBACK(on_drag_data_get), self);
}


static GtkWidget*
get_image_box (TTXWindow *self, GtkWidget *img)
{
	GtkWidget *ebox;

	ebox = gtk_event_box_new ();

	setup_dnd (self, ebox);

	gtk_widget_add_events (ebox,
			       GDK_BUTTON_PRESS_MASK |
			       GDK_POINTER_MOTION_MASK|
			       GDK_LEAVE_NOTIFY_MASK);

	g_signal_connect (G_OBJECT(ebox), "button-press-event",
			  G_CALLBACK(on_button_press_event), self);
	g_signal_connect (G_OBJECT(ebox), "motion-notify-event",
			  G_CALLBACK(on_motion_notify_event), self);

	gtk_container_add (GTK_CONTAINER(ebox), img);

	return ebox;
}


static void
ttx_window_constructed (GObject *obj)
{
	GtkWidget *vbox, *ebox;
	TTXWindow *self;

	self = TTX_WINDOW(obj);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	self->priv->toolbar = get_toolbar (self);
	self->priv->image   = gtk_image_new ();
	gtk_widget_set_size_request (self->priv->image, 440, 336);

	ebox = get_image_box (self, self->priv->image);

	gtk_box_pack_start (GTK_BOX(vbox), ebox,TRUE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX(vbox), self->priv->toolbar,
			    FALSE, FALSE, 0);

	gtk_widget_show_all (vbox);

	gtk_container_add (GTK_CONTAINER(self), vbox);

}


static void
ttx_window_init (TTXWindow *self)
{
	self->priv = TTX_WINDOW_GET_PRIVATE(self);

	self->priv->prov_id     = NULL;
	self->priv->on_link	= FALSE;
	self->priv->img_file    = NULL;

	self->priv->settings    = g_settings_new (TTX_APP_ID);

	gtk_window_set_title (GTK_WINDOW(self), _("TTX Teletext Browser"));
	gtk_window_set_resizable (GTK_WINDOW(self), FALSE);

	gtk_window_set_icon_name (GTK_WINDOW(self), "ttx");


}

static void
ttx_window_finalize (GObject *obj)
{
	TTXWindow *self;

	self = TTX_WINDOW(obj);

	free_links (self);
	reset_img_file (self, NULL);

	if (self->priv->settings)
		g_object_unref (self->priv->settings);

	g_free (self->priv->prov_id);

	G_OBJECT_CLASS(parent_class)->finalize (G_OBJECT(self));
}

GtkWidget*
ttx_window_new (TTXProviderMgr *prov_mgr)
{
	g_return_val_if_fail (prov_mgr, NULL);

	return GTK_WIDGET(g_object_new(TTX_TYPE_WINDOW,
				       "provider-mgr", prov_mgr,
				       NULL));
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
ttx_window_request_page (TTXWindow *self,
			 TTXProviderID prov_id, unsigned page,
			 unsigned subpage)
{
	const char		*combo_id;
	GtkComboBox		*combo;
	const TTXProvider	*prov;

	g_return_if_fail (TTX_IS_WINDOW(self));
	g_return_if_fail (page >= 100 && page <= 999);
	g_return_if_fail (subpage > 0);

	if (page > 999 || subpage < 1) {
		g_warning ("invalid page %u/%u", page, subpage);
		update_entry (self);
	}

	/* if not provided, get the last one (or the default) */
	if (!prov_id)
		prov_id = get_provider_id (self);

	if (prov_id) {
		prov = ttx_provider_mgr_get_provider (
			self->priv->prov_mgr, prov_id);
		if (!prov) {
			g_printerr (_("unknown teletext provider %s\n"),
				    prov_id);
			prov_id = "nos"; /* default */
		}
		
	}
	
	combo	 = GTK_COMBO_BOX(self->priv->combo);
	combo_id = gtk_combo_box_get_active_id (combo);

	self->priv->retrieving = TRUE;

	if (g_strcmp0 (prov_id, combo_id) != 0)
		if (!gtk_combo_box_set_active_id (combo, prov_id))
			g_warning ("failed to set active id to %s", prov_id);

	ttx_provider_mgr_retrieve
		(self->priv->prov_mgr,
		 prov_id,
		 page, subpage,
		 (TTXProviderResultFunc)on_completed,
		 self);
}
