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
#include <QtCore/QUuid>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtQml/QQmlEngine>

#include "globaldata.h"
#include "globalsettings.h"

// clang-format off
#define GROUP_ARRAY_ITEM    "item"
#define ITEM_INDEX          "index"

#define GAMES_GROUP         "GAMES_LIST"
#define PLAY_HOME           "home"
#define PLAY_AWAY           "away"
#define PLAY_DATETIME       "datetime"
#define PLAY_SAISON_INDEX   "sIndex"
#define PLAY_SCORE          "score"
#define PLAY_COMPETITION    "competition"
#define PLAY_TIME_FIXED     "timeFixed"

#define SEASONTICKET_GROUP  "SEASONTICKET_LIST"
#define TICKET_NAME         "name"
#define TICKET_PLACE        "place"
#define TICKET_DISCOUNT     "discount"
#define TICKET_USER_INDEX   "userIndex"

#define APP_INFO_GROUP      "AppInfo"
#define APP_INFO_TOKEN      "FcmToken"
#define APP_INFO_GUID       "AppGuid"
// clang-format on


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
    QQmlEngine::setObjectOwnership(&this->m_awayTripInfo, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&this->m_favoriteGame, QQmlEngine::CppOwnership);

    this->m_pMainUserSettings = new QSettings();
    this->m_pMainUserSettings->setIniCodec(("UTF-8"));

    QMutexLocker lock(&this->m_pushNotificationMutex);
    this->m_pMainUserSettings->beginGroup(APP_INFO_GROUP);
#ifdef Q_OS_ANDROID
    this->m_pushNotificationInfoHandler = new PushNotificationInformationHandler(this);
    connect(this->m_pushNotificationInfoHandler, &PushNotificationInformationHandler::fcmRegistrationTokenChanged,
            this, &GlobalData::slotNewFcmRegistrationToken);

    this->m_pushNotificationToken = this->m_pMainUserSettings->value(APP_INFO_TOKEN, "").toString();
#endif
    this->m_AppInstanceGUID = this->m_pMainUserSettings->value(APP_INFO_GUID, "").toString();
    if (this->m_AppInstanceGUID == "") {
        this->m_AppInstanceGUID = QUuid::createUuid().toString();
        this->m_pMainUserSettings->setValue(APP_INFO_GUID, this->m_AppInstanceGUID);
        qInfo().noquote() << QString("Create a new GUID for this instance %1").arg(this->m_AppInstanceGUID);
    }

    this->m_pMainUserSettings->endGroup();
}

void GlobalData::loadGlobalSettings()
{
    this->m_bIpAdressWasSet = false;
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

    this->resetNewsDataLastServerUpdate();

    this->m_favoriteGame.initialize();
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

    if (this->m_gpLastServerUpdateTimeStamp == timestamp && !this->m_bGamePlayLastUpdateDidChanges) {
        if (!g_GlobalSettings->saveInfosOnApp())
            return;
        this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
        this->m_pMainUserSettings->setValue("LocalGamesUpdateTime", this->m_gpLastLocalUpdateTimeStamp);
        this->m_pMainUserSettings->endGroup();
        return;
    }
    this->m_gpLastServerUpdateTimeStamp = timestamp;

    std::sort(this->m_lGamePlay.begin(), this->m_lGamePlay.end(), GamePlay::compareTimeStampFunctionAscending);

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

QString GlobalData::userName()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_userName;
}
void GlobalData::setUserName(const QString& user)
{
    if (this->m_userName != user) {
        {
            QMutexLocker lock(&this->m_mutexUser);
            this->m_userName = user;
        }
        emit userNameChanged();
    }
}

QString GlobalData::readableName()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_readableName;
}
void GlobalData::setReadableName(const QString& name)
{
    if (this->m_readableName != name) {
        {
            QMutexLocker lock(&this->m_mutexUser);
            this->m_readableName = name;
        }
        emit readableNameChanged();
    }
}

QString GlobalData::passWord()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_passWord;
}
void GlobalData::setPassWord(const QString& passw)
{
    if (this->m_passWord != passw) {
        {
            QMutexLocker lock(&this->m_mutexUser);
            this->m_passWord = passw;
        }
        emit passWordChanged();
    }
}

QString GlobalData::ipAddr()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_ipAddress;
}
void GlobalData::setIpAddr(const QString& ip)
{
    if (this->m_ipAddress != ip) {
        {
            QMutexLocker lock(&this->m_mutexUser);
            this->m_ipAddress = ip;
        }
        emit ipAddrChanged();
    }
}

quint32 GlobalData::conMasterPort()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_uMasterPort;
}
void GlobalData::setConMasterPort(quint32 port)
{
    if (this->m_uMasterPort != port) {
        {
            QMutexLocker lock(&this->m_mutexUser);
            this->m_uMasterPort = port;
        }
        emit conMasterPortChanged();
    }
}

quint32 GlobalData::conDataPort()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_uDataPort;
}
void GlobalData::setConDataPort(quint32 port)
{
    QMutexLocker lock(&this->m_mutexUser);
    if (this->m_uDataPort != port) {
        this->m_uDataPort = port;
    }
}

quint32 GlobalData::userIndex()
{
    QMutexLocker lock(&this->m_mutexUser);
    return this->m_userIndex;
}
void GlobalData::setUserIndex(quint32 userIndex)
{
    QMutexLocker lock(&this->m_mutexUser);
    if (this->m_userIndex != userIndex) {
        this->m_userIndex = userIndex;
        PushNotificationInformationHandler::setUserIndexForTopics(QString::number(userIndex));
    }
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

    this->m_gpLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();
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

void GlobalData::resetAllGamePlayEvents()
{
    for (int i = 0; i < this->m_lGamePlay.count(); i++) {
        this->m_lGamePlay[i]->setEvent(0);
    }
}

bool GlobalData::setGamePlayItemHasEvent(quint32 gameIndex)
{
    for (int i = 0; i < this->m_lGamePlay.count(); i++) {
        if (this->m_lGamePlay[i]->index() == gameIndex) {
            this->m_lGamePlay[i]->setEvent(this->m_lGamePlay[i]->getEvent() + 1);
            return true;
        }
    }
    return false;
}

void GlobalData::saveCurrentSeasonTickets(qint64 timestamp)
{
    QMutexLocker lock(&this->m_mutexTicket);

    if (this->m_stLastServerUpdateTimeStamp == timestamp && !this->m_bSeasonTicketLastUpdateDidChanges) {
        if (!g_GlobalSettings->saveInfosOnApp())
            return;
        this->m_pMainUserSettings->beginGroup(SEASONTICKET_GROUP);
        this->m_pMainUserSettings->setValue("LocalTicketsUpdateTime", this->m_stLastLocalUpdateTimeStamp);
        this->m_pMainUserSettings->endGroup();
        return;
    }

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

    this->m_stLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();
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

void GlobalData::resetSeasonTicketLastServerUpdate()
{
    this->m_stLastServerUpdateTimeStamp = 0;
}

void GlobalData::saveCurrentNewsDataList(qint64 timestamp)
{
    QMutexLocker lock(&this->m_mutexNewsData);

    this->m_ndLastServerUpdateTimeStamp = timestamp;

    std::sort(this->m_lNewsDataItems.begin(), this->m_lNewsDataItems.end(), NewsDataItem::compareTimeStampFunctionDescending);

    /* Data is not saved */
}

void GlobalData::resetNewsDataLastServerUpdate()
{
    this->m_ndLastServerUpdateTimeStamp = 0;
}

void GlobalData::startUpdateNewsDataItem(const quint16 updateIndex)
{
    QMutexLocker lock(&this->m_mutexNewsData);

    if (updateIndex == UpdateIndex::UpdateAll) {
        /* need to delete, because they are all pointers */
        for (int i = 0; i < this->m_lNewsDataItems.size(); i++)
            delete this->m_lNewsDataItems[i];
        this->m_lNewsDataItems.clear();
    }

    this->m_ndLastLocalUpdateTimeStamp = QDateTime::currentMSecsSinceEpoch();
}

void GlobalData::addNewNewsDataItem(NewsDataItem* pItem, const quint16 updateIndex)
{
    NewsDataItem* Item = this->getNewsDataItem(pItem->index());
    if (Item == NULL) {
        QMutexLocker lock(&this->m_mutexNewsData);
        this->m_lNewsDataItems.append(pItem);
        return;
    } else if (updateIndex == UpdateIndex::UpdateDiff) {
        if (Item->user() != pItem->user())
            Item->setUser(pItem->user());

        if (Item->header() != pItem->header())
            Item->setHeader(pItem->header());

        if (Item->info() != pItem->info())
            Item->setInfo(pItem->info());
    }
}

NewsDataItem* GlobalData::createNewNewsDataItem(quint32 newsIndex, QString header, QString info)
{
    NewsDataItem* pItem = this->getNewsDataItem(newsIndex);
    if (pItem != NULL) {
        pItem->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
        pItem->setHeader(header);
        pItem->setInfo(info);
        pItem->setUser(this->m_readableName);

        std::sort(this->m_lNewsDataItems.begin(), this->m_lNewsDataItems.end(), NewsDataItem::compareTimeStampFunctionDescending);
        return pItem;
    }

    pItem = new NewsDataItem();
    pItem->setIndex(newsIndex);
    pItem->setTimeStamp(QDateTime::currentMSecsSinceEpoch());
    pItem->setHeader(header);
    pItem->setInfo(info);
    pItem->setUser(this->m_readableName);

    QQmlEngine::setObjectOwnership(pItem, QQmlEngine::CppOwnership);
    this->addNewNewsDataItem(pItem, UpdateIndex::UpdateAll);

    std::sort(this->m_lNewsDataItems.begin(), this->m_lNewsDataItems.end(), NewsDataItem::compareTimeStampFunctionDescending);
    return pItem;
}

NewsDataItem* GlobalData::getNewsDataItemFromArrayIndex(int index)
{
    QMutexLocker lock(&this->m_mutexNewsData);

    if (index < this->m_lNewsDataItems.size()) {
        return this->m_lNewsDataItems.at(index);
    }
    return NULL;
}

NewsDataItem* GlobalData::getNewsDataItem(quint32 newsIndex)
{
    QMutexLocker lock(&this->m_mutexNewsData);

    for (int i = 0; i < this->m_lNewsDataItems.size(); i++) {
        if (this->m_lNewsDataItems[i]->index() == newsIndex)
            return this->m_lNewsDataItems[i];
    }
    return NULL;
}

QString GlobalData::getNewsDataLastLocalUpdateString()
{
    QMutexLocker lock(&this->m_mutexNewsData);
    return QDateTime::fromMSecsSinceEpoch(this->m_ndLastLocalUpdateTimeStamp).toString("dd.MM.yy hh:mm:ss");
}

qint64 GlobalData::getNewsDataLastLocalUpdate()
{
    QMutexLocker lock(&this->m_mutexNewsData);
    return this->m_ndLastLocalUpdateTimeStamp;
}

qint64 GlobalData::getNewsDataLastServerUpdate()
{
    QMutexLocker lock(&this->m_mutexNewsData);
    return this->m_ndLastServerUpdateTimeStamp;
}

void GlobalData::resetAllNewsDataEvents()
{
    for (int i = 0; i < this->m_lNewsDataItems.count(); i++) {
        this->m_lNewsDataItems[i]->setEvent(0);
    }
}

bool GlobalData::setNewsDataItemHasEvent(quint32 newsIndex)
{
    for (int i = 0; i < this->m_lNewsDataItems.count(); i++) {
        if (this->m_lNewsDataItems[i]->index() == newsIndex) {
            this->m_lNewsDataItems[i]->setEvent(this->m_lNewsDataItems[i]->event() + 1);
            return true;
        }
    }
    return false;
}

QString GlobalData::getCurrentLoggingList(int index)
{
    return this->m_logApp->getCurrentLoggingList(index);
}

QStringList GlobalData::getCurrentLogFileList()
{
    return this->m_logApp->getLogFileDates();
}

void GlobalData::deleteCurrentLoggingFile(int index)
{
    this->m_logApp->clearCurrentLoggingList(index);
}
void GlobalData::copyTextToClipBoard(QString text)
{
    QClipboard* clip = QGuiApplication::clipboard();
    clip->setText(text);
}

void GlobalData::callBackLookUpHost(const QHostInfo& host)
{
    QString newChangedIP;
    QString currentIP = this->ipAddr();
    if (host.addresses().size() > 0) {
        this->setIpAddr(host.addresses().value(0).toString());
        newChangedIP = host.addresses().value(0).toString();
    }

    if (g_GlobalSettings->debugIP() != "") {
        this->setIpAddr(g_GlobalSettings->debugIP());
        newChangedIP = g_GlobalSettings->debugIP();
    }
#ifdef QT_DEBUG
//    if (this->m_debugIP != "") {
//        this->setIpAddr(this->m_debugIP);
//        newChangedIP = this->m_debugIP;
//    }
#ifdef Q_OS_ANDROID

    if (g_GlobalSettings->debugIPWifi() != "") {
        QNetworkConfigurationManager ncm;
        QList<QNetworkConfiguration> nc = ncm.allConfigurations();
        foreach (QNetworkConfiguration item, nc) {
            if (item.bearerType() == QNetworkConfiguration::BearerWLAN) {
                if (item.state() == QNetworkConfiguration::StateFlag::Active) {
                    this->setIpAddr(g_GlobalSettings->debugIPWifi());
                    newChangedIP = g_GlobalSettings->debugIPWifi();
                }
                //                 qDebug() << "Wifi " << item.name();
                //                 qDebug() << "state " << item.state();
            }
        }
    }
#endif // ANDROID
#endif // DEBUG

    if (newChangedIP.isEmpty())
        newChangedIP = this->ipAddr();

    //    if (host.addresses().size() > 0)
    qInfo().noquote() << QString("Setting IP Address: %1").arg(newChangedIP) << QThread::currentThreadId();

    if (currentIP != newChangedIP)
        this->saveGlobalUserSettings();
    this->m_bIpAdressWasSet = true;
}

#ifdef Q_OS_ANDROID
void GlobalData::slotNewFcmRegistrationToken(QString token)
{
    QMutexLocker lock(&this->m_pushNotificationMutex);

    this->m_pushNotificationToken = token;

    this->m_pMainUserSettings->beginGroup(APP_INFO_GROUP);
    this->m_pMainUserSettings->setValue(APP_INFO_TOKEN, this->m_pushNotificationToken);
    this->m_pMainUserSettings->endGroup();

    qInfo().noquote() << QString("Global Data got a new token %1").arg(token);
}
#endif

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
bool GlobalData::userIsFanclubEnabled()
{
    return USER_IS_ENABLED(USER_ENABLE_FANCLUB);
}
bool GlobalData::userIsFanclubEditEnabled()
{
    return USER_IS_ENABLED(USER_ENABLE_FANCLUB_EDIT);
}

void GlobalData::SetUserProperties(quint32 value)
{
    this->m_UserProperties = value;
}
