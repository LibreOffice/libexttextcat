#
# Apr 1, 2003 frank@wise-guys.nl
# Apr 2, 2003 gnarl@wise-guys.nl
#
#--------------------------------------------------------------------------
# Macro Variables
#--------------------------------------------------------------------------
#MDEBUG		=-DMEMDEBUG -DMEMVERBOSE
#
SRC		=/home/src
RANLIB		=ranlib
RM		=/bin/rm -f
CC		=gcc
LIBS		=
LEX		=flex
LDFLAGS		=-g
OPTS		=-static -O3 -DWGTIMER -D_THREAD_SAFE -D_GNU_SOURCE -march=i686 \
		 -funroll-loops --pipe -D__GNU_C__
VERBOSE		= -DVERBOSE
IFLAGS		=

WARNS		=-W -Winline -Wall -Wshadow -Wno-unused -Wpointer-arith 

CFLAGS		=$(VERBOSE) $(IFLAGS) $(LDFLAGS) -D_GNU_SOURCE -D_REENTRANT -DWGTIMER $(WARNS) $(OPTS) $(MDEBUG)  

OBJECTS2	=	fingerprint.o textcat.o common.o wg_mempool.o
OBJECTS		=	testtextcat.o
OBJECTS3	=	createfp.o

SOURCES		=	fingerprint.c textcat.c common.c wg_mempool.c testtextcat.c createfp.c

LIBS1 		=	-lm 

LIBS 		= $(LIBS1)

#--------------------------------------------------------------------------
# Dependencies
#--------------------------------------------------------------------------
all: textcat testtextcat createfp

$(OBJECTS) : Makefile

textcat: $(OBJECTS2)
	$(RM) -f libtextcat.a ; ar cr libtextcat.a $(OBJECTS2) ; $(RANLIB) libtextcat.a

testtextcat: $(OBJECTS) libtextcat.a
	$(CC) $(CFLAGS) -static -o $@ $(OBJECTS) libtextcat.a $(LDFLAGS) $(LIBS) 

createfp: $(OBJECTS3) libtextcat.a
	$(CC) $(CFLAGS) -static -o $@ $(OBJECTS3) libtextcat.a $(LDFLAGS) $(LIBS) 

dep:	depend
#
depend: 
	makedepend -- $(CFLAGS) -- $(SOURCES)
#
$(SOURCES): Makefile
#
clean:
	$(RM) core *.o *~ testtextcat createfp *.a

# DO NOT DELETE
