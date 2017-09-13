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

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>

#include <iostream>

#include "../Common/General/globalfunctions.h"
#include "games.h"

Games::Games()
{
    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/Games.ini";

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for Games setting"));
        return;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString home      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString          away        = this->m_pConfigSettings->value(PLAY_AWAY, "").toString();
            quint8           saisonIndex = quint8(this->m_pConfigSettings->value(PLAY_SAISON_INDEX, 0).toUInt());
            quint16          saison      = quint16(this->m_pConfigSettings->value(PLAY_SAISON, 0).toUInt());
            QString          score       = this->m_pConfigSettings->value(PLAY_SCORE, "").toString();
            CompetitionIndex competition = CompetitionIndex(this->m_pConfigSettings->value(PLAY_COMPETITION, 0).toUInt());
            qint64           lastUpdate  = this->m_pConfigSettings->value(PLAY_LAST_UDPATE, 0).toLongLong();
            quint32          scheduled   = this->m_pConfigSettings->value(PLAY_SCHEDULED, false).toUInt();

            if (saison == 0) {
                bProblems  = true;
                QDate date = QDateTime::fromMSecsSinceEpoch(timestamp).date();
                if (date.month() >= 7)
                    saison = date.year();
                else
                    saison = date.year() - 1;
            }

            GamesPlay* play = new GamesPlay(home, away, timestamp, saisonIndex, score, competition, saison, index, lastUpdate, scheduled);

            if (!this->addNewGamesPlay(play))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }
    if (this->readLastUpdateTime() == 0)
        this->setNewUpdateTime();

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems        = true;
        GamesPlay* pGame = (GamesPlay*)(this->getProblemItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        pGame->m_index = this->getNextInternalIndex();
        this->addNewGamesPlay(pGame);
        //        delete pGame;
    }
    this->m_lAddItemProblems.clear();

    this->sortItemListByTime();

    if (bProblems)
        this->saveCurrentInteralList();
}

int Games::addNewGame(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, CompetitionIndex comp, quint16 saison, qint64 lastUpdate)
{
    if (sIndex == 0 || comp == NO_COMPETITION) {
        qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
        return ERROR_CODE_COMMON;
    }

    if (saison == 0) {
        QDate date = QDateTime::fromMSecsSinceEpoch(timestamp).date();
        if (date.month() >= 6)
            saison = date.year();
        else
            saison = date.year() - 1;
    }

    if (lastUpdate == 0)
        lastUpdate = QDateTime::currentMSecsSinceEpoch();

    GamesPlay* pGame;
    if ((pGame = this->gameExists(sIndex, comp, saison, timestamp)) != NULL) {
        //        QString info = QString("%1 : %2").arg(sIndex).arg(comp);
        //        qInfo() << (QString("Game \"%1\" already exists, updating info").arg(info));

        if (pGame->m_lastUpdate > lastUpdate)
            return ERROR_CODE_IN_PAST;

        this->m_mInternalInfoMutex.lock();

        if (pGame->m_itemName != home) {
            if (this->updateItemValue(pGame, ITEM_NAME, QVariant(home)))
                pGame->m_itemName = home;
        }
        if (pGame->m_away != away) {
            if (this->updateItemValue(pGame, PLAY_AWAY, QVariant(away)))
                pGame->m_away = away;
        }
        if (pGame->m_timestamp != timestamp) {
            if (this->updateItemValue(pGame, ITEM_TIMESTAMP, QVariant(timestamp))) {
                pGame->m_timestamp = timestamp;
            }

            this->m_mInternalInfoMutex.unlock();
            this->sortItemListByTime();
            this->m_mInternalInfoMutex.lock();
        }
        if (pGame->m_score != score && score.size() > 0) {
            if (this->updateItemValue(pGame, PLAY_SCORE, QVariant(score)))
                pGame->m_score = score;
        }

        if (pGame->m_competition != comp) {
            if (this->updateItemValue(pGame, PLAY_COMPETITION, QVariant(comp)))
                pGame->m_competition = comp;
        }

        if (pGame->m_saisonIndex != sIndex) {
            if (this->updateItemValue(pGame, PLAY_SAISON_INDEX, QVariant(sIndex)))
                pGame->m_saisonIndex = sIndex;
        }

        if (this->getLastUpdateTime() > lastUpdate)
            lastUpdate = this->getLastUpdateTime();

        if (this->updateItemValue(pGame, PLAY_LAST_UDPATE, QVariant(lastUpdate), lastUpdate))
            pGame->m_lastUpdate = lastUpdate;

        this->m_mInternalInfoMutex.unlock();
        return pGame->m_index;
    }

    int newIndex = this->getNextInternalIndex();

    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, home);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(PLAY_AWAY, away);
    this->m_pConfigSettings->setValue(PLAY_SAISON_INDEX, sIndex);
    this->m_pConfigSettings->setValue(PLAY_SCORE, score);
    this->m_pConfigSettings->setValue(PLAY_SAISON, saison);
    this->m_pConfigSettings->setValue(PLAY_COMPETITION, comp);
    this->m_pConfigSettings->setValue(PLAY_LAST_UDPATE, lastUpdate);
    this->m_pConfigSettings->setValue(PLAY_SCHEDULED, 0);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_mConfigIniMutex.unlock();

    GamesPlay* play = new GamesPlay(home, away, timestamp, sIndex, score, comp, saison, newIndex, lastUpdate, false);

    this->addNewGamesPlay(play, false);

    this->sortItemListByTime();

    this->setNewUpdateTime();

    qInfo().noquote() << QString("Added new game: %1").arg(home + " : " + away);
    return newIndex;
}

int Games::showAllGames()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        QString date   = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString update = QDateTime::fromMSecsSinceEpoch(pGame->m_lastUpdate).toString("dd.MM.yyyy hh:mm");
        QString output;
        if (pGame->m_score.size() > 0)
            output = QString("%1: %2 - %3 %4 - %5 = %6\t %7")
                         .arg(pGame->m_saisonIndex, 2, 10, QChar('0'))
                         .arg(pGame->m_competition)
                         .arg(date, pGame->m_itemName, pGame->m_away, pGame->m_score, update);
        else
            output = QString("%1: %2 - %3 %4 - %5\t %6")
                         .arg(pGame->m_saisonIndex, 2, 10, QChar('0'))
                         .arg(pGame->m_competition)
                         .arg(date, pGame->m_itemName, pGame->m_away, update);
        if (pGame->m_scheduled)
            output.append(" *");
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

int Games::changeScheduledValue(const quint32 gameIndex, const quint32 fixedTime)
{
    GamesPlay* gPlay = (GamesPlay*)this->getItem(gameIndex);
    if (gPlay == NULL)
        return ERROR_CODE_NOT_FOUND;

    if (gPlay->m_scheduled != fixedTime) {
        if (this->updateItemValue(gPlay, PLAY_SCHEDULED, QVariant(fixedTime))) {
            gPlay->m_scheduled = fixedTime;
            qint64 lastUpdate  = QDateTime::currentMSecsSinceEpoch();
            if (this->updateItemValue(gPlay, PLAY_LAST_UDPATE, QVariant(lastUpdate)))
                gPlay->m_lastUpdate = lastUpdate;
        } else
            return ERROR_CODE_COMMON;
    }
    return ERROR_CODE_SUCCESS;
}

void Games::saveCurrentInteralList()
{
    this->m_mConfigIniMutex.lock();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pItem = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;

        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(PLAY_AWAY, pItem->m_away);
        this->m_pConfigSettings->setValue(PLAY_SAISON_INDEX, pItem->m_saisonIndex);
        this->m_pConfigSettings->setValue(PLAY_SAISON, pItem->m_saison);
        this->m_pConfigSettings->setValue(PLAY_SCORE, pItem->m_score);
        this->m_pConfigSettings->setValue(PLAY_COMPETITION, pItem->m_competition);
        this->m_pConfigSettings->setValue(PLAY_LAST_UDPATE, pItem->m_lastUpdate);
        this->m_pConfigSettings->setValue(PLAY_SCHEDULED, pItem->m_scheduled);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->m_mConfigIniMutex.unlock();

    this->setNewUpdateTime();

    qDebug().noquote() << QString("saved current Games List with %1 entries").arg(this->getNumberOfInternalList());
}

GamesPlay* Games::gameExists(quint8 sIndex, CompetitionIndex comp, quint16 saison, qint64 timestamp)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    QDateTime date = QDateTime::fromMSecsSinceEpoch(timestamp);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        if (pGame->m_saisonIndex == sIndex && pGame->m_competition == comp && pGame->m_saison == saison) {
            QDateTime oldData = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp);
            if (date.date().year() == oldData.date().year())
                return pGame;
        }
        /* Game also exists when it is the exact timestamp, to change wrong competition or seasonIndex */
        if (pGame->m_timestamp == timestamp && (pGame->m_competition == comp || pGame->m_saisonIndex == sIndex))
            return pGame;
    }
    return NULL;
}

bool Games::addNewGamesPlay(GamesPlay* play, bool checkItem)
{
    if (checkItem) {
        if (play->m_saisonIndex == 0 || play->m_competition == NO_COMPETITION) {
            qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
            return false;
        }
        if (play->m_index == 0 || itemExists(play->m_index)) {
            qWarning().noquote() << QString("Game \"%1\" with index \"%2\" already exists, saving with new index").arg(play->m_itemName + " - " + play->m_away).arg(play->m_index);
            this->addNewConfigItem(play, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(play, &this->m_lInteralList);
    return true;
}

Games::~Games()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
