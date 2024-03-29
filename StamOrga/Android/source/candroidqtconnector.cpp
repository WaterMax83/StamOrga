/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "candroidqtconnector.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroidExtras>
#endif

static cAndroidQtConnector* g_PushInstance = nullptr;

#ifdef Q_OS_ANDROID
static QString sendFcmToken = "";
#endif

cAndroidQtConnector::cAndroidQtConnector(QObject* parent)
    : QObject(parent)
{
    this->m_fcmToken = "";
    g_PushInstance   = this;

#ifdef Q_OS_ANDROID
    if (sendFcmToken != "") {
        this->m_fcmToken = sendFcmToken;
    }
#endif
}

cAndroidQtConnector::~cAndroidQtConnector()
{
}

void cAndroidQtConnector::setNewRegistrationToken(const QString& token)
{
    if (this->m_fcmToken != token) {
        this->m_fcmToken = token;
        emit this->fcmRegistrationTokenChanged(this->m_fcmToken);
    }
}


#ifdef Q_OS_ANDROID
static void fcmTokenResult(JNIEnv* /*env*/ env, jobject obj, jstring fcmToken)
{
    const char* nativeString = env->GetStringUTFChars(fcmToken, 0);
    Q_UNUSED(obj);

    qInfo() << "Got new fcmTokenResult " << nativeString;

    if (g_PushInstance != nullptr)
        g_PushInstance->setNewRegistrationToken(QString(nativeString));
    else
        sendFcmToken = QString(nativeString);
}


static JNINativeMethod methods[] = {
    {
        "sendFCMToken", // const char* function name;
        "(Ljava/lang/String;)V",
        (void*)fcmTokenResult // function pointer
    }
};


// this method is called automatically by Java VM
// after the .so file is loaded
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6)
        != JNI_OK) {
        return JNI_ERR;
    }

    jclass javaClass = env->FindClass("org/qtproject/example/JavaNatives");
    if (!javaClass)
        return JNI_ERR;

    if (env->RegisterNatives(javaClass, methods,
                             sizeof(methods) / sizeof(methods[0]))
        < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
#endif

#include <QtCore/QDebug>
void cAndroidQtConnector::subscribeToTopic(const QString& topic)
{
#ifdef Q_OS_ANDROID
#ifdef QT_DEBUG
    topic.append("Debug");
#endif
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(topic);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/MainActivity",
                                              "SubscribeToTopic",
                                              "(Ljava/lang/String;)V",
                                              javaNotification.object<jstring>());
#else
    Q_UNUSED(topic);

    qInfo() << "Subscribe to " << topic;
#endif
}

void cAndroidQtConnector::unSubscribeFromTopic(const QString& topic)
{
#ifdef Q_OS_ANDROID
#ifdef QT_DEBUG
    topic.append("Debug");
#endif

    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(topic);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/MainActivity",
                                              "UnRegisterFromTopic",
                                              "(Ljava/lang/String;)V",
                                              javaNotification.object<jstring>());
#else
    Q_UNUSED(topic);

    qInfo() << "Unsubscribe from " << topic;
#endif
}

void cAndroidQtConnector::setUserIndexForTopics(const QString& userIndex)
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(userIndex);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/MainActivity",
                                              "SetUserIndexForNotificationTopic",
                                              "(Ljava/lang/String;)V",
                                              javaNotification.object<jstring>());
#else
    Q_UNUSED(userIndex);
#endif
}


/* This is to install an app with SDK >= 24, but it fails with parsing problem */
//void cAndroidQtConnector::installApp(const QString& appPackageName)
//{
//#ifdef Q_OS_ANDROID
//    QAndroidJniObject appName = QAndroidJniObject::fromString(appPackageName);
//    QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/JavaQtConnector",
//                                              "installApp",
//                                              "(Ljava/lang/String;)I",
//                                              appName.object<jstring>());
//#endif
//}
