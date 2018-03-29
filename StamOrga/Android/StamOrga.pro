##########################################################################################
#	File:		StamOrga.pro
#	Project:	StamOrga
#
#	Brief:		project file for StamOrga app
#	Author:		msc
#	Date:		05.04.2017
#
###########################################################################################


QT += qml quick network widgets charts

android{
    QT += androidextras
}

CONFIG += c++11

DEFINES += STAMORGA_APP

include (../../StamOrga.pri)

VERSION=$${STAMORGA_VERSION}

SOURCES += main.cpp \
	userinterface.cpp \
    ../ConnectionHandling.cpp \
    ../mainconnection.cpp \
    ../../Common/General/backgroundcontroller.cpp \
    ../../Common/General/backgroundworker.cpp \
    ../../Common/General/logging.cpp \
    ../../Common/Network/messagebuffer.cpp \
    ../../Common/Network/messagecommand.cpp \
    ../../Common/Network/messageprotocol.cpp \
    ../../Common/General/globalfunctions.cpp \
    ../../Common/General/cgendisposer.cpp \
    ../Connection/cconmanager.cpp \
    ../Connection/ccontcpmain.cpp \
    ../Connection/ccontcpdata.cpp \
    ../Connection/cconusersettings.cpp \
    ../cstaglobalmanager.cpp \
    ../cstasettingsmanager.cpp \
    ../cstaglobalsettings.cpp \
    ../Data/cdatappinfomanager.cpp \
    ../dataconnection.cpp \
    ../datahandling.cpp \
    ../Data/globaldata.cpp \
    ../Data/gameplay.cpp \
    ../Data/newsdataitem.cpp \
    ../Data/seasonticket.cpp \
    ../Data/meetinginfo.cpp \
    ../Data/globalsettings.cpp \
    ../Data/appuserevents.cpp \
    source/pushnotification.cpp \
    ../Data/gameuserdata.cpp \
    ../Data/cdataticketmanager.cpp  \
    ../Data/cdatanewsdatamanager.cpp \
    ../Data/cdatastatisticmanager.cpp \
    ../Data/cdatagamesmanager.cpp


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    userinterface.h \
    ../connectionhandling.h \
    ../mainconnection.h \
    ../../Common/General/backgroundcontroller.h \
    ../../Common/General/backgroundworker.h \
    ../../Common/General/config.h \
    ../../Common/General/logging.h \
    ../../Common/General/globaltiming.h \
    ../../Common/Network/messagebuffer.h \
    ../../Common/Network/messagecommand.h \
    ../../Common/Network/messageprotocol.h \
    ../../Common/General/globalfunctions.h \
    ../../Common/General/cgendisposer.h \
    ../Connection/cconmanager.h \
    ../Connection/ccontcpmain.h \
    ../Connection/ccontcpdata.h \
    ../Connection/cconusersettings.h \
    ../cstaglobalmanager.h \
    ../cstasettingsmanager.h \
    ../cstaglobalsettings.h \
    ../Data/cdatappinfomanager.h \
    ../dataconnection.h \
    ../datahandling.h \
    ../Data/globaldata.h \
    ../Data/gameplay.h \
    ../Data/seasonticket.h \
    ../Data/meetinginfo.h \
    ../Data/newsdataitem.h \
    ../Data/acceptmeetinginfo.h \
    ../Data/globalsettings.h \
    ../Data/appuserevents.h \
    source/pushnotification.h \
    ../Data/gameuserdata.h \
    ../Data/cdataticketmanager.h \
    ../Data/cdatanewsdatamanager.h \
    ../Data/cdatastatisticmanager.h \
    ../Data/cdatagamesmanager.h

INCLUDEPATH += \
    ../ \
    ../../Common/ \

android{
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    ################# adapt that accordingly #######################
    ANDROID_JAVA_SOURCES.path = /src/org/qtproject/example
    ################################################################

    ANDROID_JAVA_SOURCES.files = $$files($$PWD/source/java/*.java)
    INSTALLS += ANDROID_JAVA_SOURCES
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/google-services.json \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
