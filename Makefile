VERSION		=2.1
DST		=libtextcat-$(VERSION)
DISTFILES	=$(DST)/ChangeLog $(DST)/LICENSE $(DST)/Makefile \
		 $(DST)/README $(DST)/TODO \
		 $(DST)/lib/libtextcat.a $(DST)/include/textcat.h \
		 $(DST)/src/Makefile $(DST)/src/*.[ch] \
		 $(DST)/langclass/

all clean dep:
	$(MAKE) -C src $@ ; \

dist: clean links
	if [ ! -L libtextcat-$(VERSION) ] ; \
	then ln -s . libtextcat-$(VERSION) ; fi ; \
	tar cf - $(DISTFILES) | gzip -c > libtextcat-$(VERSION).tar.gz ; \
	rm -f libtextcat-$(VERSION)

links:
	if [ ! -L lib/libtextcat.a ] ; \
	then cd lib ; ln -s ../src/libtextcat.a . ; cd .. ; fi ; \
	if [ ! -L include/textcat.h ] ; \
	then cd include ; ln -s ../src/textcat.h . ; cd .. ; fi 
