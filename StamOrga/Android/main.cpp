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

#include <QtCore/QDateTime>
#include <QtCore/QMetaObject>
#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "../../Common/General/backgroundcontroller.h"
#include "../../Common/General/globalfunctions.h"
#include "../Data/globaldata.h"
#include "../dataconnection.h"
#include "../loggingapp.h"
#include "userinterface.h"


LoggingApp* g_LoggingApp = NULL;

void stamOrgaMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString newMessage;
    newMessage.append(qFormatLogMessage(type, context, msg));
    if (g_LoggingApp != NULL)
        g_LoggingApp->addNewEntry(newMessage);
}

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
    qmlRegisterType<GamePlay>("com.watermax.demo", 1, 0, "GamePlay");
    qRegisterMetaType<GamePlay*>("GamePlay*");
    qRegisterMetaType<SeasonTicketItem*>("SeasonTicketItem*");
    qRegisterMetaType<DataConRequest>("DataConRequest");

    GlobalData globalUserData;
#ifdef QT_DEBUG
    LoggingApp* loggingApp = new LoggingApp();
    loggingApp->initialize(&globalUserData);
    BackgroundController logCtrl;
    logCtrl.Start(loggingApp, false);
#endif

#ifdef QT_DEBUG
#ifdef Q_OS_ANDROID
    g_LoggingApp = loggingApp;
    qInstallMessageHandler(stamOrgaMessageOutput);
#endif
#endif

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

    QObject* pRootObject = engine.rootObjects().first();
    if (globalUserData.userName().size() == 0 || globalUserData.passWord().size() == 0)
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, true));
    else
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, false));


    int result = app.exec();
#ifdef QT_DEBUG
    loggingApp->terminate();
    logCtrl.Stop();
#endif
    return result;
}
