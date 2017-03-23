#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "userinterface.h"
#include "../../Common/General/globalfunctions.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");

    // Set the global Message Pattern
    SetMessagePattern();

    QQmlApplicationEngine engine;

//    MainConnect maincon;

//    engine.rootContext()->setContextProperty("MainConnect", &maincon);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
