WARNS =		-W -Wall -Wshadow -Wpointer-arith 
IFLAGS =
FLAGS =		-D_THREAD_SAFE -D_GNU_SOURCE
VERBOSE	=	-DVERBOSE
AM_CFLAGS =	$(IFLAGS) $(VERBOSE) $(WARNS) $(FLAGS)
AM_LDFLAGS =	-Wl,--as-needed -no-undefined

noinst_HEADERS = \
	common_impl.h wg_mempool.h

libtextcat_includedir = $(includedir)/libtextcat
libtextcat_include_HEADERS = \
	common.h constants.h fingerprint.h textcat.h utf8misc.h

lib_LTLIBRARIES =	libtextcat.la
libtextcat_la_SOURCES = \
	common.c fingerprint.c textcat.c wg_mempool.c utf8misc.c

bin_PROGRAMS =		createfp
createfp_SOURCES =	createfp.c
createfp_LDADD =	libtextcat.la

check_PROGRAMS =	testtextcat
testtextcat_SOURCES =	testtextcat.c
testtextcat_LDADD =	libtextcat.la
