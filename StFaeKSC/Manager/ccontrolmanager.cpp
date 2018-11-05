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

#include <QtCore/QJsonArray>

#include "../Common/General/globalfunctions.h"
#include "../Common/Network/messagecommand.h"
#include "ccontrolmanager.h"
#include "csmtpmanager.h"
#include "cstatisticmanager.h"
#include "General/pushnotification.h"

// clang-format off
#define GROUP_STATS             "Statistic"

#define GROUP_ONLINE_GAMES      "OnlineGames"
#define ONL_GAM_MAX_INDEX       "maxIndex"
#define ONL_GAM_SEASON          "season"

#define GROUP_SMTP              "smtp"
#define SMTP_LOGIN              "login"
#define SMTP_PASSW              "password"
#define SMTP_DEACTIVATE         "Deactivate"

// clang-format on

cControlManager g_ControlManager;

cControlManager::cControlManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cControlManager::initialize()
{
    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/ControlManager.ini";

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for Games setting"));
        return ERROR_CODE_NOT_READY;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_STATS);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            qint32 year = this->m_pConfigSettings->value(ITEM_NAME, -1).toInt();
            if (year < MIN_GAME_YEAR || year > MAX_GAME_YEAR)
                bProblems = true;
            else
                this->m_statistic.append(year);
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();

        /************** READ ONLINE GAMES ********************/
        this->m_pConfigSettings->beginGroup(GROUP_ONLINE_GAMES);
        sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString comp   = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint32  index  = this->m_pConfigSettings->value(ONL_GAM_MAX_INDEX, -1).toInt();
            qint32  season = this->m_pConfigSettings->value(ONL_GAM_SEASON, -1).toInt();
            if (comp.isEmpty() || index <= 0 || season < MIN_GAME_YEAR || season > MAX_GAME_YEAR)
                bProblems = true;
            else {
                OnlineGameCtrl* pCtrl = new OnlineGameCtrl();
                pCtrl->m_game         = new ReadOnlineGames();
                pCtrl->m_game->initialize(comp, index, season);
                pCtrl->m_ctrl.Start(pCtrl->m_game, false);
                this->m_onlineGames.append(pCtrl);
            }
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();

        /************** SMTP ********************/
        this->m_pConfigSettings->beginGroup(GROUP_SMTP);

        g_SmtpManager.setServerEmail(this->m_pConfigSettings->value(SMTP_LOGIN, "").toString());
        g_SmtpManager.setServerPassword(this->m_pConfigSettings->value(SMTP_PASSW, "").toString());

        if (this->m_pConfigSettings->value(SMTP_DEACTIVATE, 0).toInt() != 0)
            g_SmtpManager.setDeactivate(true);

        sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString addr = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            if (addr.isEmpty())
                bProblems = true;
            else
                g_SmtpManager.addDestinationEmail(addr);
        }
        this->m_pConfigSettings->endArray();

        this->m_pConfigSettings->endGroup();
    }
    if (this->readLastUpdateTime() == 0)
        this->setNewUpdateTime();

    if (bProblems)
        this->saveCurrentInteralList();

    this->startAllControls();

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}

void cControlManager::saveCurrentInteralList()
{
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_STATS);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->m_statistic.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, this->m_statistic.at(i));
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_pConfigSettings->beginGroup(GROUP_ONLINE_GAMES);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->m_onlineGames.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, this->m_onlineGames.at(i)->m_game->getCompetition());
        this->m_pConfigSettings->setValue(ONL_GAM_MAX_INDEX, this->m_onlineGames.at(i)->m_game->getMaxIndex());
        this->m_pConfigSettings->setValue(ONL_GAM_SEASON, this->m_onlineGames.at(i)->m_game->getSeason());
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();


    this->m_pConfigSettings->beginGroup(GROUP_SMTP);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->setValue(SMTP_LOGIN, g_SmtpManager.getServerEmail());
    this->m_pConfigSettings->setValue(SMTP_PASSW, g_SmtpManager.getServerPassword());

    QStringList addrs = g_SmtpManager.getDestinationEmails();
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < addrs.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, addrs.at(i));
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime();

    qInfo().noquote() << QString("saved current Control List");
}

MessageProtocol* cControlManager::getControlCommandResponse(UserConData* pUserCon, MessageProtocol* request)
{
    if (!this->m_initialized)
        return NULL;

    QByteArray  data    = QByteArray(request->getPointerToData());
    QJsonObject rootObj = QJsonDocument::fromJson(data).object();

    QString cmd = rootObj.value("cmd").toString();

    QJsonObject rootAns;
    qint32      result = ERROR_CODE_NOT_FOUND;

    if (cmd == "refresh")
        result = this->handleRefreshCommand(rootAns);
    else if (cmd == "save")
        result = this->handleSaveCommand(rootObj);
    else if (cmd == "notify")
        result = this->handleNotifyCommand(rootObj);

    rootAns.insert("ack", result);
    rootAns.insert("cmd", cmd);
    qInfo().noquote() << QString("Handle control cmd of type %1 with %3 for %2").arg(cmd, pUserCon->m_userName).arg(result);

    QByteArray answer = QJsonDocument(rootAns).toJson(QJsonDocument::Compact);

    return new MessageProtocol(OP_CODE_CMD_RES::ACK_CMD_CONTROL, answer);
}

qint32 cControlManager::handleRefreshCommand(QJsonObject& rootAns)
{
    QJsonArray  statsYearsArr;
    QJsonArray  readOnlGameArr;
    QJsonObject smtpObj;

    foreach (qint32 stat, this->m_statistic)
        statsYearsArr.append(stat);

    foreach (OnlineGameCtrl* pCtrl, this->m_onlineGames) {
        QJsonObject gameObj;
        gameObj.insert("comp", pCtrl->m_game->getCompetition());
        gameObj.insert("index", pCtrl->m_game->getMaxIndex());
        gameObj.insert("season", pCtrl->m_game->getSeason());

        readOnlGameArr.append(gameObj);
    }

    QJsonArray  addrArr;
    QStringList addrs = g_SmtpManager.getDestinationEmails();
    foreach (QString addr, addrs)
        addrArr.append(addr);
    smtpObj.insert("login", g_SmtpManager.getServerEmail());
    smtpObj.insert("password", g_SmtpManager.getServerPassword());
    smtpObj.insert("addresses", addrArr);

    rootAns.insert("stats", statsYearsArr);
    rootAns.insert("readOnlineGame", readOnlGameArr);
    rootAns.insert("smtp", smtpObj);

    return ERROR_CODE_SUCCESS;
}

qint32 cControlManager::handleSaveCommand(QJsonObject& rootObj)
{
    QJsonArray  statsArr   = rootObj.value("stats").toArray();
    QJsonArray  readOnlArr = rootObj.value("readOnlineGame").toArray();
    QJsonObject smtpObj    = rootObj.value("smtp").toObject();

    this->stopAllControls();

    for (int i = 0; i < statsArr.size(); i++) {
        qint32 year = statsArr.at(i).toInt();
        if (year > MIN_GAME_YEAR && year < MAX_GAME_YEAR) {
            this->m_statistic.append(year);
        }
    }

    for (int i = 0; i < readOnlArr.size(); i++) {
        QJsonObject gameObj  = readOnlArr.at(i).toObject();
        QString     comp     = gameObj.value("comp").toString();
        qint32      maxIndex = gameObj.value("index").toInt(-1);
        qint32      season   = gameObj.value("season").toInt(-1);

        if (comp.isEmpty() || maxIndex <= 0 || season < MIN_GAME_YEAR || season > MAX_GAME_YEAR)
            continue;
        else {
            OnlineGameCtrl* pCtrl = new OnlineGameCtrl();
            pCtrl->m_game         = new ReadOnlineGames();
            pCtrl->m_game->initialize(comp, maxIndex, season);
            pCtrl->m_ctrl.Start(pCtrl->m_game, false);
            this->m_onlineGames.append(pCtrl);
        }
    }

    g_SmtpManager.setServerEmail(smtpObj.value("login").toString());
    g_SmtpManager.setServerPassword(smtpObj.value("password").toString());
    QJsonArray addrArr = smtpObj.value("addresses").toArray();
    for (int i = 0; i < addrArr.size(); i++)
        g_SmtpManager.addDestinationEmail(addrArr.at(i).toString());

    this->saveCurrentInteralList();
    this->startAllControls();

    return ERROR_CODE_SUCCESS;
}

qint32 cControlManager::handleNotifyCommand(QJsonObject &rootObj)
{
    QString  header   = rootObj.value("header").toString();
    QString  body = rootObj.value("body").toString();
    QString  bigText = rootObj.value("bigText").toString();

    qint64 result = g_pushNotify->sendNewGeneralTopicNotification(header, body, bigText);
    if (result < 0)
        return (qint32) result;
    return ERROR_CODE_SUCCESS;
}

void cControlManager::startAllControls()
{
    foreach (qint32 year, this->m_statistic)
        g_StatisticManager.addYearToStatistic(year);
}

void cControlManager::stopAllControls()
{
    foreach (qint32 year, this->m_statistic)
        g_StatisticManager.removeYearFromStatistic(year);
    this->m_statistic.clear();

    for (int i = 0; i < this->m_onlineGames.size(); i++) {
        this->m_onlineGames.at(i)->m_game->terminate();
        this->m_onlineGames.at(i)->m_ctrl.Stop();
        delete this->m_onlineGames.at(i);
    }
    this->m_onlineGames.clear();

    g_SmtpManager.setServerEmail("");
    g_SmtpManager.setServerPassword("");
    g_SmtpManager.clearDestinationEmails();
}
