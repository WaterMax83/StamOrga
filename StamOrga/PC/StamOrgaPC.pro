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
    ../../Common/Network/messagebuffer.cpp \
    ../../Common/Network/messageprotocol.cpp \
    ../../Common/Network/messagecommand.cpp \
    ../../Common/General/backgroundcontroller.cpp \
    ../../Common/General/backgroundworker.cpp \
    ../../Common/General/globalfunctions.cpp \
    ../../Common/General/cgendisposer.cpp \
    ../Connection/cconmanager.cpp \
    ../Connection/ccontcpmain.cpp \
    ../Connection/ccontcpdata.cpp \
    ../Connection/cconsettings.cpp \
    ../cstaglobalmanager.cpp \
    ../cstasettingsmanager.cpp \



HEADERS  += mainwindow.h \
    ../../Common/Network/messagebuffer.h \
    ../../Common/Network/messageprotocol.h \
    ../../Common/General/globaltiming.h \
    ../../Common/Network/messagecommand.h \
    ../../Common/General/backgroundcontroller.h \
    ../../Common/General/backgroundworker.h \
    ../../Common/General/globalfunctions.h \
    ../../Common/General/config.h \
    ../../Common/General/cgendisposer.h \
    ../Connection/cconmanager.h \
    ../Connection/ccontcpmain.h \
    ../Connection/ccontcpdata.h \
    ../Connection/cconsettings.h \
    ../cstaglobalmanager.h \
    ../cstasettingsmanager.h \



INCLUDEPATH += \
    ../ \
    ../../Common/ \

FORMS    += mainwindow.ui
