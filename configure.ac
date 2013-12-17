## Copyright (C) 2013 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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

AC_PREREQ([2.67])
AC_INIT([ttx],[0.0.5],[],[ttx])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/ttx.c])
AC_CONFIG_MACRO_DIR([m4])

# libtoolize wants to put some stuff in here; if you have an old
# autotools/libtool setup. you can try to comment this out
AM_INIT_AUTOMAKE([1.10 no-define foreign])

# silent build if we have a new enough automake
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AS_IF([test "x$prefix" = "xNONE"],[prefix=/usr/local])
AC_SUBST(prefix)

AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_HEADER_STDC

LT_INIT

AC_CHECK_FUNCS([strcasestr])
AC_CHECK_LIB([m],[floor])

# require pkg-config
AC_PATH_PROG([PKG_CONFIG], [pkg-config], [no])
AS_IF([test "x$PKG_CONFIG" = "xno"],[
   AC_MSG_ERROR([
   *** The pkg-config script could not be found. Make sure it is
   *** in your path, or set the PKG_CONFIG environment variable
   *** to the full path to pkg-config.])
])

####################################################################
# gettext
IT_PROG_INTLTOOL([0.35.0])
ALL_LINGUAS=`(cd "$srcdir/po" > /dev/null && ls *.po) | sed 's+\.po$++'`
AM_GLIB_GNU_GETTEXT
AC_SUBST(GETTEXT_PACKAGE,[ttx])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])
AC_SUBST(TTX_LOCALEDIR,[${datadir}/locale])
####################################################################

AC_DEFINE_UNQUOTED([TTX_APP_ID], ["nl.djcbsoftware.ttx"], [application-id])

# glib2?
PKG_CHECK_MODULES(GLIB,glib-2.0 >= 2.32 gobject-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
glib_version="`$PKG_CONFIG --modversion glib-2.0`"

GLIB_GSETTINGS

PKG_CHECK_MODULES(GIO,gio-2.0 gio-unix-2.0)
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)
gio_version="`$PKG_CONFIG --modversion gio-2.0`"

PKG_CHECK_MODULES(GTK,gtk+-3.0)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
gtk_version="`$PKG_CONFIG --modversion gtk+-3.0`"

PKG_CHECK_MODULES(GDK_PIXBUF,gdk-pixbuf-2.0)
AC_SUBST(GDK_PIXBUF_CFLAGS)
AC_SUBST(GDK_PIXBUF_LIBS)
gdk_pixbuf_version="`$PKG_CONFIG --modversion gdk-pixbuf-2.0`"

PKG_CHECK_MODULES(LIBSOUP,libsoup-2.4 >= 2.42)
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)
libsoup_version="`$PKG_CONFIG --modversion libsoup-2.4`"

PKG_CHECK_MODULES(LIBXML,libxml-2.0)
AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)
libxml_version="`$PKG_CONFIG --modversion libxml-2.0`"

AC_CONFIG_FILES([
po/Makefile.in
Makefile
src/Makefile
src/providers/Makefile
ttx-gtk.desktop])
AC_OUTPUT

echo
echo "ttx $VERSION configuration is complete."
echo "------------------------------------------------"
echo "GLib version                         : $glib_version"
echo "Gio version                          : $gio_version"
echo "GTK+ version                         : $gtk_version"
echo "GDK-Pixbuf version                   : $gdk_pixbuf_version"
echo "Soup version                         : $libsoup_version"
echo "LibXML2 version                      : $libxml_version"
echo "------------------------------------------------"
echo
echo "Now, type 'make' to build ttx"
echo
echo "*** Note: you need to install it before you can run it,"
echo "*** due to GSettings"
