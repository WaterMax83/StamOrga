##########################################################################################
#	File:		StFaeKSC.pro
#	Project:	StamOrga
#
#	Brief:		project file for StFaeKSC server
#	Author:		msc
#	Date:		05.04.2017
#
###########################################################################################



QT += core network
QT -= gui

CONFIG += c++11

TARGET = StFaeKSC
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


include (../StamOrga.pri)

VERSION=$${STAMORGA_VERSION}


SOURCES += main.cpp \
    ../Common/General/backgroundcontroller.cpp \
    ../Common/General/backgroundworker.cpp \
    ../Common/General/logging.cpp \
    ../Common/General/globalfunctions.cpp \
    ../Common/General/cgendisposer.cpp \
    ../Common/Network/messagebuffer.cpp \
    ../Common/Network/messageprotocol.cpp \
    ../Common/Network/messagecommand.cpp \
    General/globaldata.cpp \
    General/console.cpp \
    General/dataconnection.cpp \
    General/pushnotification.cpp \
    Network/udpdataserver.cpp \
    Network/udpserver.cpp \
    Data/listeduser.cpp \
    Data/games.cpp \
    Data/readdatacsv.cpp \
    Data/seasonticket.cpp \
    Data/configlist.cpp \
    Data/readonlinegames.cpp \
    Data/availablegameticket.cpp \
    Data/meetinginfo.cpp \
    Data/fanclubnews.cpp \
    Data/userevents.cpp \
    Data/awaytripinfo.cpp \
    Data/checkconsistentdata.cpp \
    Network/ccontcpmainserver.cpp \
    Network/ccontcpmainsocket.cpp \
    Network/ccontcpmaindata.cpp \
    Network/ccontcpdataserver.cpp \
    Manager/cticketmanager.cpp \
    Manager/cglobalmanager.cpp \
    Manager/cnewsdatamanager.cpp \
    Manager/cstatisticmanager.cpp \
    Manager/cgamesmanager.cpp \
    Manager/cmeetinginfomanager.cpp \
    Manager/ccontrolmanager.cpp


HEADERS += \
    ../Common/General/backgroundcontroller.h \
    ../Common/General/backgroundworker.h \
    ../Common/General/config.h \
    ../Common/General/cgendisposer.h \
    ../Common/General/globaltiming.h \
    ../Common/General/logging.h \
    ../Common/General/globalfunctions.h \
    ../Common/Network/messagebuffer.h \
    ../Common/Network/messageprotocol.h \
    ../Common/Network/messagecommand.h \
    General/globaldata.h \
    General/console.h \
    General/usercommand.h \
    General/dataconnection.h \
    General/pushnotification.h \
    Network/udpdataserver.h \
    Network/connectiondata.h \
    Network/udpserver.h \
    Data/listeduser.h \
    Data/games.h \
    Data/readdatacsv.h \
    Data/seasonticket.h \
    Data/configlist.h \
    Data/readonlinegames.h \
    Data/availablegameticket.h \
    Data/meetinginfo.h \
    Data/fanclubnews.h \
    Data/userevents.h \
    Data/awaytripinfo.h \
    Data/checkconsistentdata.h \
    Network/ccontcpmainserver.h \
    Network/ccontcpmainsocket.h \
    Network/ccontcpmaindata.h \
    Network/ccontcpdataserver.h \
    Manager/cticketmanager.h \
    Manager/cglobalmanager.h \
    Manager/cnewsdatamanager.h \
    Manager/cstatisticmanager.h \
    Manager/cgamesmanager.h \
    Manager/cmeetinginfomanager.h \
    Manager/ccontrolmanager.h

unix {
#    QMAKE_POST_LINK = mkdir -p $$OUTPUT_FOLDER && cp $$TARGET $$OUTPUT_FOLDER
    QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"

    target.path = /home/pi/StFaeKSC
    INSTALLS += target

}
