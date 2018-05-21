##########################################################################################
#	File:		StamOrga.pri
#	Project:	StamOrga
#
#	Brief:		common project file for all StamOrga projects
#	Author:		msc
#	Date:		12.07.2017
#
###########################################################################################


STAMORGA_VERSION=1.1.0

DEFINES += STAMORGA_VERSION=\\\"$${STAMORGA_VERSION}\\\"

HEADERS += \
    $$PWD/StamOrga/Data/cdatacommentitem.h

SOURCES += \
    $$PWD/StamOrga/Data/cdatacommentitem.cpp
