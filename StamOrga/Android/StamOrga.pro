QT += qml quick network

CONFIG += c++11

SOURCES += main.cpp \
	userinterface.cpp \
    ../ConnectionHandling.cpp \
    ../mainconnection.cpp \
    ../../Common/General/backgroundcontroller.cpp \
    ../../Common/General/backgroundworker.cpp \
    ../../Common/Network/messagebuffer.cpp \
    ../../Common/Network/messagecommand.cpp \
    ../../Common/Network/messageprotocol.cpp \
    ../../Common/General/globalfunctions.cpp \
    ../dataconnection.cpp \
    ../datahandling.cpp \
    ../Data/globaldata.cpp \
    ../Data/gameplay.cpp \
    ../Data/seasonticket.cpp

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
    ../../Common/General/globaltiming.h \
    ../../Common/Network/messagebuffer.h \
    ../../Common/Network/messagecommand.h \
    ../../Common/Network/messageprotocol.h \
    ../../Common/General/globalfunctions.h \
    ../dataconnection.h \
    ../datahandling.h \
    ../Data/globaldata.h \
    ../Data/gameplay.h \
    ../Data/seasonticket.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
