## Copyright (C) 2008-2012 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software Foundation,
## Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

SUBDIRS=providers

AM_CPPFLAGS=-I${top_srcdir} $(GLIB_CFLAGS) $(GTK_CFLAGS)		\
	$(GIO_CFLAGS) $(GDK_PIXBUF_CFLAGS) $(LIBSOUP_CFLAGS)		\
	$(LIBXML_CFLAGS) -DICONDIR='"$(icondir)"'			\
	-DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED	\
	-DGDK_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED		\
	-DG_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES	\
	-DTTX_LOCALEDIR='"$(TTX_LOCALEDIR)"'

AM_CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wdeclaration-after-statement

bin_PROGRAMS=					\
	ttx-gtk

ttx_gtk_SOURCES=				\
	ttx.c					\
	ttx-http.c				\
	ttx-http.h				\
	ttx-link.c				\
	ttx-link.h				\
	ttx-window.c				\
	ttx-window.h				\
	ttx-provider.c				\
	ttx-provider.h				\
	ttx-provider-helpers.c			\
	ttx-provider-helpers.h			\
	ttx-provider-mgr.c			\
	ttx-provider-mgr.h			\
	ttx-provider-type-1.c			\
	ttx-provider-type-1.h			\
	ttx-provider-type-2.c			\
	ttx-provider-type-2.h


ttx_gtk_LDADD=					\
	${GLIB_LIBS}				\
	${GTK_LIBS}				\
	$(GDK_PIXBUF_LIBS)			\
	$(LIBSOUP_LIBS)				\
	$(LIBXML_LIBS)				\
	$(GIO_LIBS)				\
	providers/libproviders.la

gsettingsschema_in_files = nl.djcbsoftware.ttx.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

EXTRA_DIST=					\
	$(gsettingsschema_in_files)

CLEANFILES=					\
	$(gsettings_SCHEMAS)
