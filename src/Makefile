#
# Apr 1, 2003 frank@wise-guys.nl
# Apr 2, 2003 gnarl@wise-guys.nl
# May 19, 2003 gnarl@wise-guys.nl
#
RM		=rm -f
LDFLAGS		=-g
RANLIB		=$(AR) -ts
WARNS		=-W -Wall -Wshadow -Wpointer-arith 
FLAGS		=-g -O3 -funroll-loops -D_THREAD_SAFE -D_GNU_SOURCE
VERBOSE		=-DVERBOSE
IFLAGS		=
CFLAGS		=$(IFLAGS) $(VERBOSE) $(WARNS) $(FLAGS)

LIBOBJS		=fingerprint.o textcat.o common.o wg_mempool.o
TSTOBJS		=testtextcat.o
CFPOBJS		=createfp.o

SOURCES		=$(shell ls *.c)

#--------------------------------------------------------------------------
# Dependencies
#--------------------------------------------------------------------------
all: libtextcat.a testtextcat createfp

$(TSTOBJS) $(LIBOBJS) $(CFPOBJS) : Makefile

libtextcat.a: $(LIBOBJS)
	$(RM) $@ ; \
	$(AR) cr $@ $^ ; \
	$(RANLIB) $@

testtextcat: $(TSTOBJS) libtextcat.a
	$(CC) $(CFLAGS) -static -o $@ $^ $(LDFLAGS)

createfp: $(CFPOBJS) libtextcat.a
	$(CC) $(CFLAGS) -static -o $@ $^ $(LDFLAGS)

dep:	depend

depend: 
	makedepend -- $(IFLAGS) -- $(SOURCES)

clean:
	$(RM) core *.o *~ testtextcat createfp *.a

# DO NOT DELETE
