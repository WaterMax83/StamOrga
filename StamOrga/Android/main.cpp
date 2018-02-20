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
#include <QtCore/QStringListModel>
#include <QtGui/QFont>
#include <QtGui/QFontDatabase>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtWidgets/QApplication>

#include <iostream>

#include "../../Common/General/backgroundcontroller.h"
#include "../../Common/General/config.h"
#include "../../Common/General/globalfunctions.h"
#include "../Data/appuserevents.h"
#include "../Data/favoritegame.h"
#include "../Data/globaldata.h"
#include "../Data/globalsettings.h"
#include "../Data/statistic.h"
#include "../dataconnection.h"
#include "userinterface.h"


GlobalSettings* g_GlobalSettings;
AppUserEvents*  g_AppUserEvents;
GlobalData*     g_GlobalData;
Statistic*      g_Statistics;

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
    qRegisterMetaType<GamePlay*>("GamePlay*");
    qRegisterMetaType<SeasonTicketItem*>("SeasonTicketItem*");
    qRegisterMetaType<MeetingInfo*>("MeetingInfo*");
    qRegisterMetaType<NewsDataItem*>("NewsDataItem*");
    qRegisterMetaType<AcceptMeetingInfo*>("AcceptMeetingInfo*");
    qRegisterMetaType<DataConRequest>("DataConRequest");
    qRegisterMetaType<FavoriteGame*>("FavoriteGame*");

    GlobalSettings globalSettings;
    g_GlobalSettings = &globalSettings;

    GlobalData globalUserData;
    g_GlobalData = &globalUserData;
    globalSettings.initialize(&globalUserData, &app);

    AppUserEvents appUserEvents;
    appUserEvents.initialize(&globalUserData);
    g_AppUserEvents = &appUserEvents;

    Statistic statistics;
    statistics.initialize();
    g_Statistics = &statistics;

    QFontDatabase base;
    QStringList   fontFamilies = base.families();
    QString       chFontFam    = globalSettings.getChangeDefaultFont();
    if (chFontFam != "Default") {

        if (fontFamilies.contains(chFontFam)) {
            qInfo().noquote() << QString("Aendere Schrift von %1 nach %2").arg(app.font().family(), chFontFam);

            QFont newFont = QFont(chFontFam);
            app.setFont(newFont);

            //            QString styleSheet = QString("font-family:%1px;").arg(chFontFam);
            //            app.setStyleSheet(styleSheet);
        } else
            qWarning().noquote() << QString("Kann Schrift nicht nach %2 aendern, da Sie nicht vorhanden ist, behalte %1")
                                        .arg(app.font().family(), chFontFam);
    }

    fontFamilies.insert(0, "Default");
    globalSettings.setCurrentFontList(&fontFamilies);
    QStringListModel fontFamiliesModel;
    fontFamiliesModel.setStringList(fontFamilies);

    // engine to start qml display -> takes about half a second
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("globalUserData", &globalUserData);
    engine.rootContext()->setContextProperty("globalSettings", &globalSettings);
    engine.rootContext()->setContextProperty("fontFamiliesModel", &fontFamiliesModel);
    engine.rootContext()->setContextProperty("appUserEvents", &appUserEvents);
    engine.rootContext()->setContextProperty("statistics", &statistics);
    //    engine.rootContext()->setContextProperty("fontFamiliesModel", QVariant::fromValue(fontFamilies));
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

    QObject* pRootObject
        = engine.rootObjects().first();
    if (globalUserData.userName().size() == 0 || globalUserData.passWord().size() == 0)
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, true));
    else
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, false));

    return app.exec();
}
