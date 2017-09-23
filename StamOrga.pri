##########################################################################################
#	File:		StamOrga.pri
#	Project:	StamOrga
#
#	Brief:		common project file for all StamOrga projects
#	Author:		msc
#	Date:		12.07.2017
#
###########################################################################################


STAMORGA_VERSION=1.0.3

DEFINES += STAMORGA_VERSION=\\\"$${STAMORGA_VERSION}\\\"

HEADERS += \
    $$PWD/StamOrga/Data/newsdataitem.h

SOURCES += \
    $$PWD/StamOrga/Data/newsdataitem.cpp
