AM_CFLAGS =	-D_THREAD_SAFE -D_GNU_SOURCE -DVERBOSE

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

EXTRA_DIST = libtextcat.map makefile.mk test.sh \
	af.test \
	am.test \
	ar.test \
	be.test \
	bg.test \
	br.test \
	cy.test \
	de.test \
	ga.test \
	gd.test \
	hu.test \
	eu.test \
	sq.test \
	uk.test \
	yi.test

TESTS = \
	af.test \
	am.test \
	ar.test \
	be.test \
	bg.test \
	br.test \
	cy.test \
	de.test \
	ga.test \
	gd.test \
	hu.test \
	eu.test \
	sq.test \
	uk.test \
	yi.test
