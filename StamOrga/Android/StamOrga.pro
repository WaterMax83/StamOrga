##########################################################################################
#	File:		StamOrga.pro
#	Project:	StamOrga
#
#	Brief:		project file for StamOrga app
#	Author:		msc
#	Date:		05.04.2017
#
###########################################################################################


QT += qml quick network widgets charts webview

android{
    QT += androidextras
}

CONFIG += c++11

DEFINES += STAMORGA_APP

include (../../StamOrga.pri)

VERSION=$${STAMORGA_VERSION}

SOURCES += main.cpp \
	userinterface.cpp \
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
    ../Connection/cconnetworkaccess.cpp \
    ../cstaglobalmanager.cpp \
    ../cstasettingsmanager.cpp \
    ../cstaglobalsettings.cpp \
    ../cstaversionmanager.cpp \
    ../Data/cdatappinfomanager.cpp \
    ../Data/gameplay.cpp \
    ../Data/seasonticket.cpp \
    ../Data/cdataticketmanager.cpp  \
    ../Data/cdatanewsdatamanager.cpp \
    ../Data/cdatastatisticmanager.cpp \
    ../Data/cdatagamesmanager.cpp \
    ../Data/cdatameetinginfo.cpp \
    ../Data/cdatagameuserdata.cpp \
    ../Data/cdataappuserevents.cpp \
    ../Data/cdataconsolemanager.cpp \
    ../Data/cdatacommentitem.cpp \
    ../Data/cdatamediamanager.cpp \
    ../Data/cdatamediaprovider.cpp \
    ../Data/cdatawebpagemanager.cpp \
    ../Data/textdataitem.cpp \
    ../Data/cdatausermanager.cpp \
    source/candroidqtconnector.cpp


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
    ../Connection/cconnetworkaccess.h \
    ../cstaglobalmanager.h \
    ../cstasettingsmanager.h \
    ../cstaglobalsettings.h \
    ../cstaversionmanager.h \
    ../Data/cdatappinfomanager.h \
    ../Data/gameplay.h \
    ../Data/seasonticket.h \
    ../Data/acceptmeetinginfo.h \
    ../Data/cdataticketmanager.h \
    ../Data/cdatanewsdatamanager.h \
    ../Data/cdatastatisticmanager.h \
    ../Data/cdatagamesmanager.h \
    ../Data/cdatameetinginfo.h \
    ../Data/cdatagameuserdata.h \
    ../Data/cdataappuserevents.h \
    ../Data/cdataconsolemanager.h \
    ../Data/cdatacommentitem.h \
    ../Data/cdatamediamanager.h \
    ../Data/cdatamediaprovider.h \
    ../Data/cdatawebpagemanager.h \
    ../Data/textdataitem.h \
    ../Data/cdatausermanager.h \
    source/candroidqtconnector.h

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
    android/res/xml/provider_paths.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

#contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#    ANDROID_EXTRA_LIBS = \
#        $$PWD/libs/libcrypto.so \
#        $$PWD/libs/libssl.so
#}
android: include(D:/Daten/Projekte/build/Android/sdk/android_openssl/openssl.pri)

ANDROID_EXTRA_LIBS = D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/arm/libcrypto_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/arm/libssl_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/arm64/libcrypto_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/arm64/libssl_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/x86/libcrypto_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/x86/libssl_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/x86_64/libcrypto_1_1.so D:/Daten/Projekte/build/Android/sdk/android_openssl/latest/x86_64/libssl_1_1.so

ANDROID_ABIS = armeabi-v7a arm64-v8a x86 x86_64
