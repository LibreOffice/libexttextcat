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
		af am ar ast be bg bn bo br bs ca cs cy da de \
		ee en eo es et eu fa fi fj fo fr fur fy ga gd \
		gl gn gu gv el ha-NG haw he hi hil hr hsb ht \
		hu hy ia id is it ja ka kk kl km ko ky ktu la \
		lb lg ln lo lt lv mai mi mg mk ml mn mr ms mt \
		my nb ne nl nn nso ny om pa pl pt qxa rm ro \
		ru rw sa sc se sg sk sl so sq sr sr-Latn ss \
		st sv sw ta tet th tg ti tk tl tn tpi tr ts \
		tt ty ug uk uz ve vi wa xh yi yo zh-Hans \
		zh-Hant zu; do \
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
