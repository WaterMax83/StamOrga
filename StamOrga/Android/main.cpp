#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QSettings>
#include <QtCore/QMetaObject>

#include "userinterface.h"
#include "../../Common/General/globalfunctions.h"
#include "../globaldata.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");

    // Set the global Message Pattern
    SetMessagePattern();

    GlobalData globalUserData;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("globalUserData", &globalUserData);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    globalUserData.loadGlobalSettings();

    if (engine.rootObjects().size() == 0) {
        qCritical() << "Warning no root qml object loaded, end programm";
        return -1;
    }

    QObject *pRootObject = engine.rootObjects().first();
    QMetaObject::invokeMethod(pRootObject, "openUserLogin");

//    if (globalUserData.userName().size() == 0) {


//    }

    return app.exec();
}
