#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T11:02:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StamOrgaPC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../Common/Network/messagebuffer.cpp \
    ../../Common/Network/messageprotocol.cpp \
    ../../Common/Network/messagecommand.cpp \
    ../../Common/General/backgroundcontroller.cpp \
    ../../Common/General/backgroundworker.cpp \
    ../connectionhandling.cpp \
    ../mainconnection.cpp \
    ../globaldata.cpp

HEADERS  += mainwindow.h \
    ../../Common/Network/messagebuffer.h \
    ../../Common/Network/messageprotocol.h \
    ../../Common/General/globaltiming.h \
    ../../Common/Network/messagecommand.h \
    ../../Common/General/backgroundcontroller.h \
    ../../Common/General/backgroundworker.h \
    ../connectionhandling.h \
    ../mainconnection.h \
    ../globaldata.h

FORMS    += mainwindow.ui
