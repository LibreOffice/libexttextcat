#*************************************************************************
#
#   $RCSfile: libtextcat-2.2.patch,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: tl $ $Date: 2007-01-12 12:34:52 $
#
#*  The Contents of this file are made available subject to
#*  the terms of GNU Lesser General Public License Version 2.1.
#*
#*
#*    GNU Lesser General Public License Version 2.1
#*    =============================================
#*    Copyright 2005 by Sun Microsystems, Inc.
#*    901 San Antonio Road, Palo Alto, CA 94303, USA
#*
#*    This library is free software; you can redistribute it and/or
#*    modify it under the terms of the GNU Lesser General Public
#*    License version 2.1, as published by the Free Software Foundation.
#*
#*    This library is distributed in the hope that it will be useful,
#*    but WITHOUT ANY WARRANTY; without even the implied warranty of
#*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#*    Lesser General Public License for more details.
#*
#*    You should have received a copy of the GNU Lesser General Public
#*    License along with this library; if not, write to the Free Software
#*    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#*    MA  02111-1307  USA
#*
#*************************************************************************

PRJ = ..$/..$/..$/..$/..

PRJNAME = libtextcat
TARGET  = libtextcat
CFLAGSCALL=gsd

USE_DEFFILE=TRUE
EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

# !! not to be compiled because those belong to a stand alone programs: !!
#        $(SLO)$/createfp.obj\
#        $(SLO)$/testtextcat.obj

SLOFILES=   \
        $(SLO)$/common.obj\
        $(SLO)$/fingerprint.obj\
        $(SLO)$/textcat.obj\
        $(SLO)$/wg_mempool.obj\
        $(SLO)$/utf8misc.obj

#SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX)
SHL1TARGET= $(TARGET)

SHL1STDLIBS=

# build DLL
SHL1LIBS=       $(SLB)$/$(TARGET).lib
SHL1IMPLIB=     i$(TARGET)
SHL1DEPN=       $(SHL1LIBS)
SHL1DEF=        $(MISC)$/$(SHL1TARGET).def

# build DEF file
DEF1NAME=       $(SHL1TARGET)
DEF1LIBNAME=$(TARGET)
DEF1DEPN=$(MISC)$/$(SHL1TARGET).flt

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

# copy hand supplied configuration file for Win32 builds to the file
# which is included in the source code
$(SLOFILES) : config.h
config.h :
    $(GNUCOPY) $(OUT)$/misc$/build$/libtextcat-2.2$/src$/win32_config.h   $(OUT)$/misc$/build$/libtextcat-2.2$/src$/config.h


$(MISC)$/$(SHL1TARGET).flt:  makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo Imp>$@
    @echo __CT>>$@
    @echo _real>>$@
    @echo unnamed>>$@
