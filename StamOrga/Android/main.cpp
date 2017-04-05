#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QSettings>
#include <QtCore/QMetaObject>
#include <QtCore/QDateTime>


#include "userinterface.h"
#include "../../Common/General/globalfunctions.h"
#include "../Data/globaldata.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Set the global Message Pattern
    SetMessagePattern();

    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
//    qmlRegisterType<GamePlay*>("com.watermax.demo", 1, 0, "GamePlay*");
    qRegisterMetaType<GamePlay *>("GamePlay*");

    // global data class
    GlobalData globalUserData;

    // engine to start qml display -> takes about half a second
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("globalUserData", &globalUserData);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    // load settings to update data
    globalUserData.loadGlobalSettings();

    if (engine.rootObjects().size() == 0) {
        qCritical() << "Warning no root qml object loaded, end programm";
        return -1;
    }

#ifdef QT_DEBUG

#endif

    QObject *pRootObject = engine.rootObjects().first();
    if (globalUserData.userName().size() == 0)
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, true));
    else
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, false));


    return app.exec();
}
