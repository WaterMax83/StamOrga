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
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <iostream>

#include "../../Common/General/backgroundcontroller.h"
#include "../../Common/General/config.h"
#include "../../Common/General/globalfunctions.h"
#include "../Data/globaldata.h"
#include "../Data/globalsettings.h"
#include "../dataconnection.h"
#include "userinterface.h"


GlobalSettings g_GlobalSettings;

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);


    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
    //    qmlRegisterType<GamePlay>("com.watermax.demo", 1, 0, "MeetingInfo*");
    qRegisterMetaType<GamePlay*>("GamePlay*");
    qRegisterMetaType<SeasonTicketItem*>("SeasonTicketItem*");
    qRegisterMetaType<MeetingInfo*>("MeetingInfo*");
    qRegisterMetaType<AcceptMeetingInfo*>("AcceptMeetingInfo*");
    qRegisterMetaType<DataConRequest>("DataConRequest");

    GlobalData globalUserData;
    g_GlobalSettings.initialize(&globalUserData);

    // engine to start qml display -> takes about half a second
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("globalUserData", &globalUserData);
    engine.rootContext()->setContextProperty("globalSettings", &g_GlobalSettings);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    // load settings to update data
    globalUserData.loadGlobalSettings();

    if (engine.rootObjects().size() == 0) {
        qCritical() << "Warning no root qml object loaded, end programm";
        return -1;
    }

    qInfo().noquote() << QString("Started StamOrga %1").arg(STAM_ORGA_VERSION_S);

#ifdef QT_DEBUG

#endif

    QObject* pRootObject = engine.rootObjects().first();
    if (globalUserData.userName().size() == 0 || globalUserData.passWord().size() == 0)
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, true));
    else
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, false));

    return app.exec();
}
