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

#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtQml/QQmlEngine>

#include "globaldata.h"

#define GROUP_ARRAY_ITEM "item"
#define ITEM_INDEX "index"

#define GAMES_GROUP "GAMES_LIST"
#define PLAY_HOME "home"
#define PLAY_AWAY "away"
#define PLAY_DATETIME "datetime"
#define PLAY_SAISON_INDEX "sIndex"
#define PLAY_SCORE "score"
#define PLAY_COMPETITION "competition"

#define SEASONTICKET_GROUP "SEASONTICKET_LIST"
#define TICKET_NAME "name"
#define TICKET_PLACE "place"
#define TICKET_DISCOUNT "discount"
#define TICKET_USER_INDEX "userIndex"


GlobalData::GlobalData(QObject* parent)
    : QObject(parent)
{
    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");
    this->setbIsConnected(false);
    this->SetUserProperties(0x0);

    this->m_logApp = new Logging();
    this->m_logApp->initialize();
    this->m_ctrlLog.Start(this->m_logApp, false);
}

void GlobalData::loadGlobalSettings()
{
    //    QMutexLocker lock(&this->m_mutexUserIni);

    QHostInfo::lookupHost("watermax83.ddns.net", this, SLOT(callBackLookUpHost(QHostInfo)));

    this->m_pMainUserSettings = new QSettings();

    qInfo() << this->m_pMainUserSettings->fileName();

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->setUserName(this->m_pMainUserSettings->value("UserName", "").toString());
    this->setPassWord(this->m_pMainUserSettings->value("Password", "").toString());
    this->setReadableName(this->m_pMainUserSettings->value("ReadableName", "").toString());
    this->setIpAddr(this->m_pMainUserSettings->value("IPAddress", "140.80.61.57").toString());
    this->setConMasterPort(this->m_pMainUserSettings->value("ConMasterPort", 55000).toInt());

    this->m_pMainUserSettings->endGroup();

    /* Getting data from last Games */
    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
    this->m_gpLastTimeStamp = this->m_pMainUserSettings->value("TIMESTAMP", 0).toLongLong();
    int count               = this->m_pMainUserSettings->beginReadArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < count; i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        GamePlay* play = new GamePlay();
        play->setHome(this->m_pMainUserSettings->value(PLAY_HOME, "").toString());
        play->setAway(this->m_pMainUserSettings->value(PLAY_AWAY, "").toString());
        play->setTimeStamp(this->m_pMainUserSettings->value(PLAY_DATETIME, 0).toLongLong());
        play->setSeasonIndex(quint8(this->m_pMainUserSettings->value(PLAY_SAISON_INDEX, 0).toUInt()));
        play->setIndex(this->m_pMainUserSettings->value(ITEM_INDEX, 0).toUInt());
        play->setScore(this->m_pMainUserSettings->value(PLAY_SCORE, "").toString());
        play->setCompetition(quint8(this->m_pMainUserSettings->value(PLAY_COMPETITION, 0).toUInt()));

        QQmlEngine::setObjectOwnership(play, QQmlEngine::CppOwnership);
        this->addNewGamePlay(play);
    }
    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();

    /* Getting data from last SeasonTickets */
    this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
    this->m_stLastTimeStamp = this->m_pMainUserSettings->value("TIMESTAMP", 0).toLongLong();

    int ticketCount = this->m_pMainUserSettings->beginReadArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < ticketCount; i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        SeasonTicketItem* ticket = new SeasonTicketItem();

        ticket->setName(this->m_pMainUserSettings->value(TICKET_NAME, "").toString());
        ticket->setPlace(this->m_pMainUserSettings->value(TICKET_PLACE, "").toString());
        ticket->setDiscount(quint8(this->m_pMainUserSettings->value(TICKET_DISCOUNT, 0).toUInt()));
        ticket->setIndex(this->m_pMainUserSettings->value(ITEM_INDEX, 0).toUInt());
        ticket->setUserIndex(this->m_pMainUserSettings->value(TICKET_USER_INDEX, 0).toUInt());

        QQmlEngine::setObjectOwnership(ticket, QQmlEngine::CppOwnership);
        this->addNewSeasonTicket(ticket);
    }

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();
}

void GlobalData::saveGlobalUserSettings()
{
    QMutexLocker lock(&this->m_mutexUser);

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->m_pMainUserSettings->setValue("UserName", this->m_userName);
    this->m_pMainUserSettings->setValue("Password", this->m_passWord);
    this->m_pMainUserSettings->setValue("ReadableName", this->m_readableName);
    this->m_pMainUserSettings->setValue("IPAddress", this->m_ipAddress);
    this->m_pMainUserSettings->setValue("ConMasterPort", this->m_uMasterPort);

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();
}

void GlobalData::saveActualGamesList()
{
    QMutexLocker lock(&this->m_mutexGame);

    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
    this->m_pMainUserSettings->remove(""); // clear all elements

    this->m_pMainUserSettings->setValue("TIMESTAMP", this->m_gpLastTimeStamp);

    this->m_pMainUserSettings->beginWriteArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < this->m_lGamePlay.size(); i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        this->m_pMainUserSettings->setValue(PLAY_HOME, this->m_lGamePlay[i]->home());
        this->m_pMainUserSettings->setValue(PLAY_AWAY, this->m_lGamePlay[i]->away());
        this->m_pMainUserSettings->setValue(PLAY_DATETIME, this->m_lGamePlay[i]->timestamp64Bit());
        this->m_pMainUserSettings->setValue(PLAY_SAISON_INDEX, this->m_lGamePlay[i]->seasonIndex());
        this->m_pMainUserSettings->setValue(ITEM_INDEX, this->m_lGamePlay[i]->index());
        this->m_pMainUserSettings->setValue(PLAY_SCORE, this->m_lGamePlay[i]->score());
        this->m_pMainUserSettings->setValue(PLAY_COMPETITION, this->m_lGamePlay[i]->compValue());
    }

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();
}

void GlobalData::startUpdateGamesPlay()
{
    QMutexLocker lock(&this->m_mutexGame);

    /* need to delet, because they are all pointers */
    for (int i = 0; i < this->m_lGamePlay.size(); i++)
        delete this->m_lGamePlay[i];
    this->m_lGamePlay.clear();
    this->m_gpLastTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GlobalData::addNewGamePlay(GamePlay* gPlay)
{
    if (!this->existGamePlay(gPlay)) {
        QMutexLocker lock(&this->m_mutexGame);
        //        qDebug() << QString("Add new game play %1:%2 = %3").arg(gPlay->home(), gPlay->away(), gPlay->score());
        this->m_lGamePlay.append(gPlay);
    }
}

bool GlobalData::existGamePlay(GamePlay* gPlay)
{
    QMutexLocker lock(&this->m_mutexGame);

    if (gPlay->index() == 0 || gPlay->competition() == 0)
        return false;

    for (int i = 0; i < this->m_lGamePlay.size(); i++) {
        if (this->m_lGamePlay[i]->index() == gPlay->index() && this->m_lGamePlay[i]->competition() == gPlay->competition() && this->m_lGamePlay[i]->timestamp() == gPlay->timestamp())
            return true;
    }
    return false;
}

GamePlay* GlobalData::getGamePlay(int index)
{
    QMutexLocker lock(&this->m_mutexGame);

    if (index < this->m_lGamePlay.size())
        return this->m_lGamePlay.at(index);
    return NULL;
}

QString GlobalData::getGamePlayLastUpdate()
{
    QMutexLocker lock(&this->m_mutexGame);
    return QDateTime::fromMSecsSinceEpoch(this->m_gpLastTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

void GlobalData::saveCurrentSeasonTickets()
{
    QMutexLocker lock(&this->m_mutexTicket);

    this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
    this->m_pMainUserSettings->remove(""); // clear all elements

    this->m_pMainUserSettings->setValue("TIMESTAMP", this->m_stLastTimeStamp);

    this->m_pMainUserSettings->beginWriteArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < this->m_lSeasonTicket.size(); i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        this->m_pMainUserSettings->setValue(TICKET_NAME, this->m_lSeasonTicket[i]->name());
        this->m_pMainUserSettings->setValue(TICKET_PLACE, this->m_lSeasonTicket[i]->place());
        this->m_pMainUserSettings->setValue(TICKET_DISCOUNT, this->m_lSeasonTicket[i]->discount());
        ;
        this->m_pMainUserSettings->setValue(ITEM_INDEX, this->m_lSeasonTicket[i]->index());
        ;
        this->m_pMainUserSettings->setValue(TICKET_USER_INDEX, this->m_lSeasonTicket[i]->userIndex());
        ;
    }

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();
}

void GlobalData::startUpdateSeasonTickets()
{
    QMutexLocker lock(&this->m_mutexTicket);

    /* need to delete, because they are all pointers */
    for (int i = 0; i < this->m_lSeasonTicket.size(); i++)
        delete this->m_lSeasonTicket[i];
    this->m_lSeasonTicket.clear();
    this->m_stLastTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GlobalData::addNewSeasonTicket(SeasonTicketItem* sTicket)
{
    if (!this->existSeasonTicket(sTicket)) {
        QMutexLocker lock(&this->m_mutexTicket);
        //        qDebug() << QString("Add new game play %1:%2 = %3").arg(gPlay->home(), gPlay->away(), gPlay->score());
        this->m_lSeasonTicket.append(sTicket);
    }
}

bool GlobalData::existSeasonTicket(SeasonTicketItem* sTicket)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (sTicket->name().size() == 0)
        return false;

    for (int i = 0; i < this->m_lSeasonTicket.size(); i++) {
        if (this->m_lSeasonTicket[i]->name() == sTicket->name())
            return true;
    }
    return false;
}

SeasonTicketItem* lastItem = NULL;

SeasonTicketItem* GlobalData::getSeasonTicket(int index)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (index < this->m_lSeasonTicket.size()) {
        if (lastItem != NULL)
            delete lastItem;

        return this->m_lSeasonTicket.at(index);
    }
    return NULL;
}

QString GlobalData::getSeasonTicketLastUpdate()
{
    QMutexLocker lock(&this->m_mutexTicket);
    return QDateTime::fromMSecsSinceEpoch(this->m_stLastTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

void GlobalData::copyTextToClipBoard(QString text)
{
    QClipboard* clip = QGuiApplication::clipboard();
    clip->setText(text);
}

void GlobalData::callBackLookUpHost(const QHostInfo& host)
{
#ifdef Q_OS_ANDROID
    if (host.addresses().size() > 0)
        this->setIpAddr(host.addresses().value(0).toString());

#ifdef QT_DEBUG
    QNetworkConfigurationManager ncm;
    QList<QNetworkConfiguration> nc = ncm.allConfigurations();

    foreach (QNetworkConfiguration item, nc) {
        if (item.bearerType() == QNetworkConfiguration::BearerWLAN) {
            if (item.state() == QNetworkConfiguration::StateFlag::Active)
                this->setIpAddr("192.168.0.35");
            //                 qDebug() << "Wifi " << item.name();
            //                 qDebug() << "state " << item.state();
        }
        //       else {
        //           qDebug() << "Network " << item.name();
        //           qDebug() << "state " << item.state();
        //       }
    }
#endif
#endif

    if (host.addresses().size() > 0)
        qDebug().noquote() << QString("Getting host info ip: %1").arg(host.addresses().value(0).toString());
}
