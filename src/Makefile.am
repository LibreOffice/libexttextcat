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

EXTRA_DIST = libtextcat.map makefile.mk test-primary.sh.in test-secondary.sh.in

check:
	@echo primary languages
	@for primarylanguage in \
		af am ar be bg br ca cs cy da de en eo es et \
		eu fa fi fr fy ga gd gv el he hi hu hy id is \
		it ka la lt lv mr ms nb ne nl nn pl pt rm ro \
		ru sa sk sl sq sv sw ta th tl tr uk vi yi; do \
		sh ./test-primary.sh $$primarylanguage; \
		if test x$$? != x0; then \
			echo FAIL: $$primarylanguage && exit 1; \
		else \
			echo PASS: $$primarylanguage; \
		fi; \
	done
	@echo secondary languages
	@for secondarylanguage in sco; do \
		sh ./test-secondary.sh $$secondarylanguage; \
		if test x$$? != x0; then \
			echo FAIL: $$secondarylanguage && exit 1; \
		else \
			echo PASS: $$secondarylanguage; \
		fi; \
	done
