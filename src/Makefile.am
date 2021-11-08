AM_CFLAGS =	-D_THREAD_SAFE -D_GNU_SOURCE -DVERBOSE

noinst_HEADERS = \
	common_impl.h wg_mempool.h

libexttextcat_2_0_includedir = $(includedir)/libexttextcat
libexttextcat_2_0_include_HEADERS = \
	common.h constants.h fingerprint.h textcat.h utf8misc.h textcat_properties.h \
        $(builddir)/exttextcat-version.h

lib_LTLIBRARIES =	libexttextcat-2.0.la
libexttextcat_2_0_la_SOURCES = \
	common.c fingerprint.c textcat.c wg_mempool.c utf8misc.c
libexttextcat_2_0_la_LDFLAGS = -no-undefined

bin_PROGRAMS =		createfp
createfp_SOURCES =	createfp.c
createfp_LDADD =	libexttextcat-2.0.la

check_PROGRAMS =	testtextcat
testtextcat_SOURCES =	testtextcat.c
testtextcat_LDADD =	libexttextcat-2.0.la

EXTRA_DIST = libexttextcat.map \
	test-primary.sh.in \
	test-secondary.sh.in \
	exttextcat-version.h  \
	exttextcat-version.h.in

check:
	@echo primary languages
	@for primarylanguage in \
		ab ace ada af ak alt am ar arn ast ay az az-Cyrl ban be \
		bem bho bi bik bg bm bn bo br bs buc ca ckb cs cv cy da \
		de dv dz ee el emk-Latn en eo es et eu fa fi fj fkv fo \
		fon fr fur fy ga gd gl grc gu gug gv ha-NG haw he hi \
		hil hr hsb ht hu hy ia id is it ja ka kbd kk kl km kn \
		kng km ko koi ktu ky la lb lg lij lld ln lo lt lv mai \
		mi mk ml mn mos mr ms mt my nb nds ne nl nn nr nso \
		ny oc om pa pl plt pt quz qxa rm ro ru rue rw sa \
		sc sd se sg shs si sk sl so sq sr-Cyrl sr-Latn \
		ss st sv sw ta tet th tg ti tk tl tn tpi tr ts \
		tt ty tzm-Latn ug uk uz uz-Cyrl ve vep vi wa xh \
		yi yo zh-Hans zh-Hant zu; do \
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
