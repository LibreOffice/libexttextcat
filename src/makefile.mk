#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ = ..$/..$/..$/..$/..

PRJNAME = libexttextcat
TARGET  = libexttextcat
CFLAGSCALL=gsd

USE_DEFFILE=TRUE
EXTERNAL_WARNINGS_NOT_ERRORS := TRUE
UWINAPILIB=

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
DEF1DEPN=$(MISC)$/$(SHL1TARGET).flt

SHL1VERSIONMAP= libexttextcat.map

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk

$(MISC)$/$(SHL1TARGET).flt:  makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo Imp>$@
    @echo __CT>>$@
    @echo _real>>$@
    @echo unnamed>>$@
