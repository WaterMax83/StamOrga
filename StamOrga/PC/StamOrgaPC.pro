#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T11:02:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StamOrgaPC
TEMPLATE = app

include (../../StamOrga.pri)

VERSION=$${STAMORGA_VERSION}


SOURCES += main.cpp\
    mainwindow.cpp \
    cpccontrolmanager.cpp \
    ../../Common/Network/messagebuffer.cpp \
    ../../Common/Network/messageprotocol.cpp \
    ../../Common/Network/messagecommand.cpp \
    ../../Common/General/backgroundcontroller.cpp \
    ../../Common/General/backgroundworker.cpp \
    ../../Common/General/globalfunctions.cpp \
    ../../Common/General/cgendisposer.cpp \
    ../../Common/General/logging.cpp \
    ../Connection/cconmanager.cpp \
    ../Connection/ccontcpmain.cpp \
    ../Connection/ccontcpdata.cpp \
    ../Connection/cconnetworkaccess.cpp \
    ../Connection/cconusersettings.cpp \
    ../cstaglobalmanager.cpp \
    ../cstasettingsmanager.cpp \
    ../cstaglobalsettings.cpp \
    ../cstaversionmanager.cpp \
    ../Data/cdatappinfomanager.cpp \
    ../Data/cdataappuserevents.cpp \
    ../Data/cdataconsolemanager.cpp \
    ../Data/cdatagamesmanager.cpp \
    ../Data/cdatagameuserdata.cpp \
    ../Data/cDataMeetingInfo.cpp \
    ../Data/cdatanewsdatamanager.cpp \
    ../Data/cdatawebpagemanager.cpp \
    ../Data/cdatastatisticmanager.cpp \
    ../Data/cdataticketmanager.cpp \
    ../Data/cdatausermanager.cpp \
    ../Data/cdatamediamanager.cpp \
    ../Data/gameplay.cpp \
    ../Data/textdataitem.cpp \
    ../Data/seasonticket.cpp \
    ../Data/cdatacommentitem.cpp


HEADERS  += mainwindow.h \
    cpccontrolmanager.h \
    ../../Common/Network/messagebuffer.h \
    ../../Common/Network/messageprotocol.h \
    ../../Common/General/globaltiming.h \
    ../../Common/Network/messagecommand.h \
    ../../Common/General/backgroundcontroller.h \
    ../../Common/General/backgroundworker.h \
    ../../Common/General/globalfunctions.h \
    ../../Common/General/config.h \
    ../../Common/General/cgendisposer.h \
    ../../Common/General/logging.h \
    ../Connection/cconmanager.h \
    ../Connection/ccontcpmain.h \
    ../Connection/ccontcpdata.h \
    ../Connection/cconnetworkaccess.h \
    ../Connection/cconusersettings.h \
    ../cstaglobalmanager.h \
    ../cstasettingsmanager.h \
    ../cstaglobalsettings.h \
    ../cstaversionmanager.h \
    ../Data/cdatappinfomanager.h \
    ../Data/acceptmeetinginfo.h \
    ../Data/cdataappuserevents.h \
    ../Data/cdataconsolemanager.h \
    ../Data/cdatagamesmanager.h \
    ../Data/cdatagameuserdata.h \
    ../Data/cdatameetinginfo.h \
    ../Data/cdatanewsdatamanager.h \
    ../Data/cdatawebpagemanager.h \
    ../Data/cdatastatisticmanager.h \
    ../Data/cdatausermanager.h \
    ../Data/cdataticketmanager.h \
    ../Data/cdatamediamanager.h \
    ../Data/gameplay.h \
    ../Data/textdataitem.h \
    ../Data/seasonticket.h \   
    ../Data/cdatacommentitem.h



INCLUDEPATH += \
    ../ \
    ../../Common/ \

FORMS    += mainwindow.ui
