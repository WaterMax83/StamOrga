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
#include "globalsettings.h"

#define GROUP_ARRAY_ITEM "item"
#define ITEM_INDEX "index"

#define GAMES_GROUP "GAMES_LIST"
#define PLAY_HOME "home"
#define PLAY_AWAY "away"
#define PLAY_DATETIME "datetime"
#define PLAY_SAISON_INDEX "sIndex"
#define PLAY_SCORE "score"
#define PLAY_COMPETITION "competition"
#define PLAY_TIME_FIXED "timeFixed"

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

    QQmlEngine::setObjectOwnership(&this->m_meetingInfo, QQmlEngine::CppOwnership);

    this->m_pMainUserSettings = new QSettings();
    this->m_pMainUserSettings->setIniCodec(("UTF-8"));
}

void GlobalData::loadGlobalSettings()
{
    //    QMutexLocker lock(&this->m_mutexUserIni);

    QHostInfo::lookupHost("watermax83.ddns.net", this, SLOT(callBackLookUpHost(QHostInfo)));

    qInfo().noquote() << this->m_pMainUserSettings->fileName();

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->setUserName(this->m_pMainUserSettings->value("UserName", "").toString());
    this->setPassWord(this->m_pMainUserSettings->value("Password", "").toString());
    this->setSalt(this->m_pMainUserSettings->value("Salt", "").toString());
    this->setReadableName(this->m_pMainUserSettings->value("ReadableName", "").toString());
    this->setIpAddr(this->m_pMainUserSettings->value("IPAddress", "140.80.61.57").toString());
    this->setConMasterPort(this->m_pMainUserSettings->value("ConMasterPort", 55000).toInt());

    this->m_pMainUserSettings->endGroup();

    if (!g_GlobalSettings->saveInfosOnApp()) {
        this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
        if (this->m_pMainUserSettings->childGroups().length() > 0 || this->m_pMainUserSettings->childKeys().length() > 0)
            this->m_pMainUserSettings->remove("");
        this->m_pMainUserSettings->endGroup();
        this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
        if (this->m_pMainUserSettings->childGroups().length() > 0 || this->m_pMainUserSettings->childKeys().length() > 0)
            this->m_pMainUserSettings->remove("");
        this->m_pMainUserSettings->endGroup();
        return;
    }

    /* Getting data from last Games */
    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
    this->m_gpLastLocalUpdateTimeStamp  = this->m_pMainUserSettings->value("LocalGamesUpdateTime", 0).toLongLong();
    this->m_gpLastServerUpdateTimeStamp = this->m_pMainUserSettings->value("ServerGamesUpdateTime", 0).toLongLong();
    int count                           = this->m_pMainUserSettings->beginReadArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < count; i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        GamePlay* play = new GamePlay();
        play->setHome(this->m_pMainUserSettings->value(PLAY_HOME, "").toString());
        play->setAway(this->m_pMainUserSettings->value(PLAY_AWAY, "").toString());
        play->setTimeStamp(this->m_pMainUserSettings->value(PLAY_DATETIME, 0).toLongLong());
        play->setSeasonIndex(quint8(this->m_pMainUserSettings->value(PLAY_SAISON_INDEX, 0).toUInt()));
        play->setIndex(this->m_pMainUserSettings->value(ITEM_INDEX, 0).toUInt());
        play->setScore(this->m_pMainUserSettings->value(PLAY_SCORE, "").toString());
        play->setCompetition(CompetitionIndex(quint8(this->m_pMainUserSettings->value(PLAY_COMPETITION, 0).toUInt())));
        play->setTimeFixed(this->m_pMainUserSettings->value(PLAY_TIME_FIXED, false).toBool());

        QQmlEngine::setObjectOwnership(play, QQmlEngine::CppOwnership);
        this->addNewGamePlay(play);
    }
    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();
    this->m_bGamePlayLastUpdateDidChanges = false;

    /* Getting data from last SeasonTickets */
    this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
    this->m_stLastLocalUpdateTimeStamp  = this->m_pMainUserSettings->value("LocalTicketsUpdateTime", 0).toLongLong();
    this->m_stLastServerUpdateTimeStamp = this->m_pMainUserSettings->value("ServerTicketsUpdateTime", 0).toLongLong();

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
    this->m_bSeasonTicketLastUpdateDidChanges = false;
}

void GlobalData::saveGlobalUserSettings()
{
    QMutexLocker lock(&this->m_mutexUser);

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->m_pMainUserSettings->setValue("UserName", this->m_userName);
    this->m_pMainUserSettings->setValue("Password", this->m_passWord);
    this->m_pMainUserSettings->setValue("Salt", this->m_salt);
    this->m_pMainUserSettings->setValue("ReadableName", this->m_readableName);
    this->m_pMainUserSettings->setValue("IPAddress", this->m_ipAddress);
    this->m_pMainUserSettings->setValue("ConMasterPort", this->m_uMasterPort);

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->sync();
}

void GlobalData::saveCurrentGamesList(qint64 timestamp)
{
    QMutexLocker lock(&this->m_mutexGame);

    if (this->m_gpLastServerUpdateTimeStamp == timestamp && !this->m_bGamePlayLastUpdateDidChanges)
        return;
    this->m_gpLastServerUpdateTimeStamp = timestamp;

    std::sort(this->m_lGamePlay.begin(), this->m_lGamePlay.end(), GamePlay::compareTimeStampFunction);

    if (!g_GlobalSettings->saveInfosOnApp())
        return;

    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
    this->m_pMainUserSettings->remove(""); // clear all elements

    this->m_pMainUserSettings->setValue("LocalGamesUpdateTime", this->m_gpLastLocalUpdateTimeStamp);
    this->m_pMainUserSettings->setValue("ServerGamesUpdateTime", this->m_gpLastServerUpdateTimeStamp);

    this->m_pMainUserSettings->beginWriteArray(GROUP_ARRAY_ITEM);
    for (int i = 0; i < this->m_lGamePlay.size(); i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        this->m_pMainUserSettings->setValue(PLAY_HOME, this->m_lGamePlay[i]->home());
        this->m_pMainUserSettings->setValue(PLAY_AWAY, this->m_lGamePlay[i]->away());
        this->m_pMainUserSettings->setValue(PLAY_DATETIME, this->m_lGamePlay[i]->timestamp64Bit());
        this->m_pMainUserSettings->setValue(PLAY_SAISON_INDEX, this->m_lGamePlay[i]->seasonIndex());
        this->m_pMainUserSettings->setValue(ITEM_INDEX, this->m_lGamePlay[i]->index());
        this->m_pMainUserSettings->setValue(PLAY_SCORE, this->m_lGamePlay[i]->score());
        this->m_pMainUserSettings->setValue(PLAY_COMPETITION, this->m_lGamePlay[i]->competitionValue());
        this->m_pMainUserSettings->setValue(PLAY_TIME_FIXED, this->m_lGamePlay[i]->timeFixed());
    }

    this->m_pMainUserSettings->endArray();
    this->m_pMainUserSettings->endGroup();
}

void GlobalData::startUpdateGamesPlay(const qint16 updateIndex)
{
    QMutexLocker lock(&this->m_mutexGame);

    /* need to delet, because they are all pointers */
    if (updateIndex == UpdateIndex::UpdateAll) {
        for (int i = 0; i < this->m_lGamePlay.size(); i++)
            delete this->m_lGamePlay[i];
        this->m_lGamePlay.clear();
        this->m_bGamePlayLastUpdateDidChanges = true;
    } else
        this->m_bGamePlayLastUpdateDidChanges = false;

    this->m_gpLastLocalUpdateTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GlobalData::addNewGamePlay(GamePlay* gPlay, const qint16 updateIndex)
{
    GamePlay* play = this->getGamePlay(gPlay->index());
    if (play == NULL) {
        QMutexLocker lock(&this->m_mutexGame);

        gPlay->setEnableAddGame(this->userIsGameAddingEnabled());
        this->m_lGamePlay.append(gPlay);
        this->m_bGamePlayLastUpdateDidChanges = true;
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (play->home() != gPlay->home()) {
            play->setHome(gPlay->home());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->away() != gPlay->away()) {
            play->setAway(gPlay->away());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->score() != gPlay->score()) {
            play->setScore(gPlay->score());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->timestamp64Bit() != gPlay->timestamp64Bit()) {
            play->setTimeStamp(gPlay->timestamp64Bit());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->seasonIndex() != gPlay->seasonIndex()) {
            play->setSeasonIndex(gPlay->seasonIndex());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->competitionValue() != gPlay->competitionValue()) {
            play->setCompetition((CompetitionIndex)gPlay->competitionValue());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
        if (play->timeFixed() != gPlay->timeFixed()) {
            play->setTimeFixed(gPlay->timeFixed());
            this->m_bGamePlayLastUpdateDidChanges = true;
        }
    }

    delete gPlay;
}

GamePlay* GlobalData::getGamePlay(const quint32 gameIndex)
{
    QMutexLocker lock(&this->m_mutexGame);

    for (int i = 0; i < this->m_lGamePlay.size(); i++) {
        if (this->m_lGamePlay[i]->index() == gameIndex)
            return this->m_lGamePlay[i];
    }
    return NULL;
}

GamePlay* GlobalData::getGamePlayFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutexGame);

    if (index < this->m_lGamePlay.size())
        return this->m_lGamePlay.at(index);
    return NULL;
}

QString GlobalData::getGamePlayLastUpdateString()
{
    QMutexLocker lock(&this->m_mutexGame);
    return QDateTime::fromMSecsSinceEpoch(this->m_gpLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

qint64 GlobalData::getGamePlayLastLocalUpdate()
{
    return this->m_gpLastLocalUpdateTimeStamp;
}

qint64 GlobalData::getGamePlayLastServerUpdate()
{
    return this->m_gpLastServerUpdateTimeStamp;
}

void GlobalData::saveCurrentSeasonTickets(qint64 timestamp)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (this->m_stLastServerUpdateTimeStamp == timestamp && !this->m_bSeasonTicketLastUpdateDidChanges)
        return;

    this->m_stLastServerUpdateTimeStamp = timestamp;

    if (!g_GlobalSettings->saveInfosOnApp())
        return;

    this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
    this->m_pMainUserSettings->remove(""); // clear all elements

    this->m_pMainUserSettings->setValue("LocalTicketsUpdateTime", this->m_stLastLocalUpdateTimeStamp);
    this->m_pMainUserSettings->setValue("ServerTicketsUpdateTime", this->m_stLastServerUpdateTimeStamp);

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

void GlobalData::startUpdateSeasonTickets(const quint16 updateIndex)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (updateIndex == UpdateIndex::UpdateAll) {
        /* need to delete, because they are all pointers */
        for (int i = 0; i < this->m_lSeasonTicket.size(); i++)
            delete this->m_lSeasonTicket[i];
        this->m_lSeasonTicket.clear();
        this->m_bSeasonTicketLastUpdateDidChanges = true;
    } else
        this->m_bSeasonTicketLastUpdateDidChanges = false;

    this->m_stLastLocalUpdateTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GlobalData::addNewSeasonTicket(SeasonTicketItem* sTicket, const quint16 updateIndex)
{
    SeasonTicketItem* ticket = this->getSeasonTicket(sTicket->index());
    if (ticket == NULL) {
        QMutexLocker lock(&this->m_mutexTicket);
        this->m_lSeasonTicket.append(sTicket);
        this->m_bSeasonTicketLastUpdateDidChanges = true;
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (ticket->name() != sTicket->name()) {
            ticket->setName(sTicket->name());
            this->m_bSeasonTicketLastUpdateDidChanges = true;
        }
        if (ticket->place() != sTicket->place()) {
            ticket->setPlace(sTicket->place());
            this->m_bSeasonTicketLastUpdateDidChanges = true;
        }
        if (ticket->discount() != sTicket->discount()) {
            ticket->setDiscount(sTicket->discount());
            this->m_bSeasonTicketLastUpdateDidChanges = true;
        }
    }
}

SeasonTicketItem* GlobalData::getSeasonTicketFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (index < this->m_lSeasonTicket.size()) {
        return this->m_lSeasonTicket.at(index);
    }
    return NULL;
}

SeasonTicketItem* GlobalData::getSeasonTicket(quint32 ticketIndex)
{
    QMutexLocker lock(&this->m_mutexTicket);

    for (int i = 0; i < this->m_lSeasonTicket.size(); i++) {
        if (this->m_lSeasonTicket[i]->index() == ticketIndex)
            return this->m_lSeasonTicket[i];
    }
    return NULL;
}

QString GlobalData::getSeasonTicketLastLocalUpdateString()
{
    QMutexLocker lock(&this->m_mutexTicket);
    return QDateTime::fromMSecsSinceEpoch(this->m_stLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

qint64 GlobalData::getSeasonTicketLastLocalUpdate()
{
    QMutexLocker lock(&this->m_mutexTicket);
    return this->m_stLastLocalUpdateTimeStamp;
}

qint64 GlobalData::getSeasonTicketLastServerUpdate()
{
    QMutexLocker lock(&this->m_mutexTicket);
    return this->m_stLastServerUpdateTimeStamp;
}

void GlobalData::copyTextToClipBoard(QString text)
{
    QClipboard* clip = QGuiApplication::clipboard();
    clip->setText(text);
}

void GlobalData::callBackLookUpHost(const QHostInfo& host)
{
    QString lastIP;
    if (host.addresses().size() > 0) {
        this->setIpAddr(host.addresses().value(0).toString());
        lastIP = host.addresses().value(0).toString();
    }

    if (g_GlobalSettings->debugIP() != "") {
        this->setIpAddr(g_GlobalSettings->debugIP());
        lastIP = g_GlobalSettings->debugIP();
    }
#ifdef QT_DEBUG
//    if (this->m_debugIP != "") {
//        this->setIpAddr(this->m_debugIP);
//        lastIP = this->m_debugIP;
//    }
#ifdef Q_OS_ANDROID

    if (g_GlobalSettings->debugIPWifi() != "") {
        QNetworkConfigurationManager ncm;
        QList<QNetworkConfiguration> nc = ncm.allConfigurations();
        foreach (QNetworkConfiguration item, nc) {
            if (item.bearerType() == QNetworkConfiguration::BearerWLAN) {
                if (item.state() == QNetworkConfiguration::StateFlag::Active) {
                    this->setIpAddr(g_GlobalSettings->debugIPWifi());
                    lastIP = g_GlobalSettings->debugIPWifi();
                }
                //                 qDebug() << "Wifi " << item.name();
                //                 qDebug() << "state " << item.state();
            }
        }
    }
#endif // ANDROID
#endif // DEBUG

    if (host.addresses().size() > 0)
        qInfo().noquote() << QString("Setting IP Address: %1").arg(lastIP);
}

bool GlobalData::userIsDebugEnabled()
{
    return USER_IS_ENABLED(USER_ENABLE_LOG);
}
bool GlobalData::userIsGameAddingEnabled()
{
    return USER_IS_ENABLED(USER_ENABLE_ADD_GAME);
}
bool GlobalData::userIsGameFixedTimeEnabled()
{
    return USER_IS_ENABLED(USER_ENABLE_FIXED_GAME_TIME);
}

void GlobalData::SetUserProperties(quint32 value)
{
    this->m_UserProperties = value;
}
