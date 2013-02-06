## Copyright (C) 2008-2012 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## t	he Free Software Foundation; either version 3 of the License, or
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

AM_CPPFLAGS=-I${top_srcdir} $(GLIB_CFLAGS) $(GTK_CFLAGS)		\
	$(GDK_PIXBUF_CFLAGS) $(LIBSOUP_CFLAGS) $(LIBXML_CFLAGS)		\
	-DICONDIR='"$(icondir)"' -DGTK_DISABLE_DEPRECATED		\
	-DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED	\
	-DPANGO_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES		\
	-DGTK_DISABLE_SINGLE_INCLUDES

AM_CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wdeclaration-after-statement

bin_PROGRAMS=					\
	ttx

ttx_SOURCES=					\
	ttx.c					\
	ttx-http.c				\
	ttx-http.h				\
	ttx-link.c				\
	ttx-link.h				\
	ttx-provider-mgr.h			\
	ttx-provider-mgr.c			\
	ttx-provider-nos-teletekst.c		\
	ttx-provider-nos-teletekst.h		\
	ttx-window.c				\
	ttx-window.h

ttx_LDADD=					\
	${GLIB_LIBS}				\
	${GTK_LIBS}				\
	$(GDK_PIXBUF_LIBS)			\
	$(LIBSOUP_LIBS)				\
	$(LIBXML_LIBS)
