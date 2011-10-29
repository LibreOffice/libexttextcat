AM_CFLAGS =	-D_THREAD_SAFE -D_GNU_SOURCE -DVERBOSE

noinst_HEADERS = \
	common_impl.h wg_mempool.h

libexttextcat_includedir = $(includedir)/libexttextcat
libexttextcat_include_HEADERS = \
	common.h constants.h fingerprint.h textcat.h utf8misc.h \
        $(builddir)/exttextcat-version.h

lib_LTLIBRARIES =	libexttextcat.la
libexttextcat_la_SOURCES = \
	common.c fingerprint.c textcat.c wg_mempool.c utf8misc.c
libexttextcat_la_LDFLAGS = -no-undefined

bin_PROGRAMS =		createfp
createfp_SOURCES =	createfp.c
createfp_LDADD =	libexttextcat.la

check_PROGRAMS =	testtextcat
testtextcat_SOURCES =	testtextcat.c
testtextcat_LDADD =	libexttextcat.la

EXTRA_DIST = libexttextcat.map \
	makefile.mk \
	test-primary.sh.in \
	test-secondary.sh.in \
	exttextcat-version.h  \
	exttextcat-version.h.in

check:
	@echo primary languages
	@for primarylanguage in \
		af am ar be bg bn bo br bs ca cs cy da de en \
		eo es et eu fa fi fr fy ga gd gu gv el he hi \
		hr hu hy id is it ja ka km ko la lb lo lt lv \
		mai ml mn mr ms my nb ne nl nn pa pl pt qxa \
		rm ro ru sa sk sl sq sr sr-Latn sv sw ta tet \
		th tg ti tk tl tn tpi tr ts tt ug uk uz ve \
		vi wa xh yi yo zh-Hans zh-Hant zu; do \
		bash ./test-primary.sh $$primarylanguage; \
		if test x$$? != x0; then \
			echo FAIL: $$primarylanguage && exit 1; \
		else \
			echo PASS: $$primarylanguage; \
		fi; \
	done
	@echo secondary languages
	@for secondarylanguage in sco; do \
		bash ./test-secondary.sh $$secondarylanguage; \
		if test x$$? != x0; then \
			echo FAIL: $$secondarylanguage && exit 1; \
		else \
			echo PASS: $$secondarylanguage; \
		fi; \
	done
