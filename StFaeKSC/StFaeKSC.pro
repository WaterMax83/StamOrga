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
    Network/udpserver.cpp \
    ../Common/Network/messagebuffer.cpp \
    ../Common/Network/messageprotocol.cpp \
    ../Common/Network/messagecommand.cpp \
    General/globaldata.cpp \
    General/console.cpp \
    Network/udpdataserver.cpp \
    ../Common/General/logging.cpp \
    ../Common/General/globalfunctions.cpp \
    General/dataconnection.cpp \
    Data/listeduser.cpp \
    Data/games.cpp \
    Data/readdatacsv.cpp \
    Data/seasonticket.cpp \
    Data/configlist.cpp \
    Data/readonlinegames.cpp \
    Data/availablegameticket.cpp \
    Data/meetinginfo.cpp \
    General/pushnotification.cpp \
    Data/fanclubnews.cpp

HEADERS += \
    ../Common/General/backgroundcontroller.h \
    ../Common/General/backgroundworker.h \
    ../Common/General/config.h \
    Network/udpserver.h \
    ../Common/Network/messagebuffer.h \
    ../Common/Network/messageprotocol.h \
    ../Common/General/globaltiming.h \
    ../Common/Network/messagecommand.h \
    General/globaldata.h \
    ../Common/General/globalfunctions.h \
    General/console.h \
    General/usercommand.h \
    Network/udpdataserver.h \
    Network/connectiondata.h \
    ../Common/General/logging.h \
    General/dataconnection.h \
    Data/listeduser.h \
    Data/games.h \
    Data/readdatacsv.h \
    Data/seasonticket.h \
    Data/configlist.h \
    Data/readonlinegames.h \
    Data/availablegameticket.h \
    Data/meetinginfo.h \
    General/pushnotification.h \
    Data/fanclubnews.h


unix {
#    QMAKE_POST_LINK = mkdir -p $$OUTPUT_FOLDER && cp $$TARGET $$OUTPUT_FOLDER
    QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"

    target.path = /home/pi/StFaeKSC
    INSTALLS += target

}
