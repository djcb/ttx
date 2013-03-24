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

AM_CPPFLAGS=-I${top_srcdir}/src $(GLIB_CFLAGS) -D_GNU_SOURCE	\
-DTTXLOCALEDIR='"$(TTXLOCALEDIR)"'
AM_CFLAGS=-Wall -Wextra -Wno-unused-parameter -Wdeclaration-after-statement

noinst_LTLIBRARIES=				\
	libproviders.la

libproviders_la_SOURCES=			\
	ttx-providers.h				\
	ttx-provider-ceefax-uk.c		\
	ttx-provider-een-be.c			\
	ttx-provider-eins-de.c			\
	ttx-provider-hrt-hr.c			\
	ttx-provider-mtv3-fi.c			\
	ttx-provider-nos-nl.c			\
	ttx-provider-orf-at.c			\
	ttx-provider-rai-it.c			\
	ttx-provider-rtp-pt.c                   \
	ttx-provider-rtve-es.c                  \
	ttx-provider-rtvslo-si.c		\
	ttx-provider-yle-fi.c

libproviders_la_LIBADD=				\
	${GLIB_LIBS}
