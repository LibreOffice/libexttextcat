ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = ChangeLog LICENSE README README.libtextcat TODO langclass

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = libtextcat.pc

# install all data flat, get rid of fpdb.conf eventually
pkgdata_DATA = langclass/fpdb.conf langclass/LM/*.lm

SUBDIRS = src
