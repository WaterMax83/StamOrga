#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QSettings>

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

    return app.exec();
}
