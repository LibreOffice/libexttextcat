AUTOMAKE_OPTIONS = 1.4 foreign

WARNS =		-W -Wall -Wshadow -Wpointer-arith 
IFLAGS =
FLAGS =		-g -O3 -funroll-loops -D_THREAD_SAFE -D_GNU_SOURCE
VERBOSE	=	-DVERBOSE
AM_CFLAGS =	$(IFLAGS) $(VERBOSE) $(WARNS) $(FLAGS)
AM_LDFLAGS =	-g

noinst_HEADERS = \
	common.h constants.h fingerprint.h textcat.h wg_mempool.h

lib_LTLIBRARIES =	libtextcat.la
libtextcat_la_SOURCES = \
	common.c fingerprint.c textcat.c wg_mempool.c

bin_PROGRAMS =		createfp
createfp_SOURCES =	createfp.c
createfp_LDADD =	libtextcat.la

noinst_PROGRAMS =	testtextcat
testtextcat_SOURCES =	testtextcat.c
testtextcat_LDADD =	libtextcat.la
