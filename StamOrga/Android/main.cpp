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

//#include <iostream>

#include "../../Common/General/backgroundcontroller.h"
#include "../../Common/General/config.h"
#include "../../Common/General/globalfunctions.h"
#include "../Connection/cconusersettings.h"
#include "../Data/cdataappuserevents.h"
#include "../Data/cdatagameuserdata.h"
#include "../Data/cdatastatisticmanager.h"
#include "../cstaglobalmanager.h"
#include "../cstaglobalsettings.h"
#include "userinterface.h"


int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QApplication app(argc, argv);

    //    // Register our component type with QML.
    //    qmlRegisterType<UserInterface>("com.watermax.demo", 1, 0, "UserInterface");
    //    qRegisterMetaType<GamePlay*>("GamePlay*");
    //    qRegisterMetaType<SeasonTicketItem*>("SeasonTicketItem*");
    //    qRegisterMetaType<cDataMeetingInfo*>("cDataMeetingInfo*");
    //    qRegisterMetaType<NewsDataItem*>("NewsDataItem*");
    //    qRegisterMetaType<AcceptMeetingInfo*>("AcceptMeetingInfo*");
    //    qRegisterMetaType<DataConRequest>("DataConRequest");
    //    //    qRegisterMetaType<GameUserData*>("GameUserData*");
    //    qRegisterMetaType<StatBars*>("StatBars*");

    cStaGlobalManager staGlobalManager;
    staGlobalManager.initialize();

    QFontDatabase base;
    QStringList   fontFamilies = base.families();
    QString       chFontFam    = g_StaGlobalSettings->getChangeDefaultFont();
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
    g_StaGlobalSettings->setCurrentFontList(&fontFamilies);
    QStringListModel fontFamiliesModel;
    fontFamiliesModel.setStringList(fontFamilies);

    // engine to start qml display -> takes about half a second
    QQmlApplicationEngine engine;
    staGlobalManager.setQmlInformationClasses(&engine);
    engine.rootContext()->setContextProperty("fontFamiliesModel", &fontFamiliesModel);
    // //    engine.rootContext()->setContextProperty("fontFamiliesModel", QVariant::fromValue(fontFamilies));
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    if (engine.rootObjects().size() == 0) {
        qCritical() << "Warning no root qml object loaded, end programm";
        return -1;
    }

    qInfo().noquote() << QString("Started StamOrga %1").arg(STAM_ORGA_VERSION_S);

#ifdef QT_DEBUG

#endif

    QObject* pRootObject = engine.rootObjects().first();
    if (g_ConUserSettings->getUserName().size() == 0 || g_ConUserSettings->getPassWord() == 0)
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, true));
    else
        QMetaObject::invokeMethod(pRootObject, "openUserLogin", Q_ARG(QVariant, false));

    return app.exec();
}
