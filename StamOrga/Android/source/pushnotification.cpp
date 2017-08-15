#include "pushnotification.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroidExtras>
#endif

static PushNotificationInformationHandler* g_PushInstance = NULL;

PushNotificationInformationHandler::PushNotificationInformationHandler(QObject* parent)
    : QObject(parent)
{
    this->m_fcmToken = "";
    g_PushInstance   = this;
}

PushNotificationInformationHandler::~PushNotificationInformationHandler()
{
}

void PushNotificationInformationHandler::setNewRegistrationToken(QString token)
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

    if (g_PushInstance != NULL)
        g_PushInstance->setNewRegistrationToken(QString(nativeString));
}


// create a vector with all our JNINativeMethod(s)
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
    // get the JNIEnv pointer.
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6)
        != JNI_OK) {
        return JNI_ERR;
    }

    // search for Java class which declares the native methods
    jclass javaClass = env->FindClass("org/qtproject/example/JavaNatives");
    if (!javaClass)
        return JNI_ERR;

    // register our native methods
    if (env->RegisterNatives(javaClass, methods,
                             sizeof(methods) / sizeof(methods[0]))
        < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
#endif
