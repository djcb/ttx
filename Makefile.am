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

SUBDIRS= po src

#ACLOCAL_AMFLAGS=-I m4

desktop_in_files=ttx.desktop.in
desktopdir=$(datadir)/applications
desktop_DATA= $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

icondir=$(datadir)/icons/hicolor/48x48/apps
icon_DATA=ttx.png

EXTRA_DIST= ttx.desktop.in ttx.png
ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES =			\
	$(srcdir)/ABOUT-NLS		\
	$(srcdir)/INSTALL		\
	$(srcdir)/aclocal.m4		\
	$(srcdir)/autoscan.log		\
	$(srcdir)/compile		\
	$(srcdir)/config.guess		\
	$(srcdir)/config.h.in		\
	$(srcdir)/config.rpath		\
	$(srcdir)/config.sub		\
	$(srcdir)/configure.scan	\
	$(srcdir)/depcomp		\
	$(srcdir)/install-sh		\
	$(srcdir)/ltmain.sh		\
	$(srcdir)/missing		\
	$(srcdir)/mkinstalldirs		\
	$(srcdir)/m4/gettext.m4		\
	$(srcdir)/m4/iconv.m4		\
	$(srcdir)/m4/lib-ld.m4		\
	$(srcdir)/m4/lib-link.m4	\
	$(srcdir)/m4/lib-prefix.m4	\
	$(srcdir)/m4/nls.m4		\
	$(srcdir)/m4/po.m4		\
	$(srcdir)/m4/progtest.m4	\
	$(srcdir)/po/Makefile.in.in~	\
	$(srcdir)/po/Makevars.template	\
	$(srcdir)/po/Rules-quot		\
	$(srcdir)/po/boldquot.sed	\
	$(srcdir)/po/en@boldquot.header	\
	$(srcdir)/po/en@quot.header	\
	$(srcdir)/po/insert-header.sin	\
	$(srcdir)/po/quot.sed		\
	$(srcdir)/po/remove-potcdate.sin
