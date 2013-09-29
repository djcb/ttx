#!/bin/sh
rm -f aclocal.m4
intltoolize --force || exit 1
autoreconf -i || exit 1
./configure

