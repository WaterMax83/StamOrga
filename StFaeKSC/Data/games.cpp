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
    QString gamesSetFilePath = getUserHomeConfigPath() + "/Settings/Games.ini";

    if (!checkFilePathExistAndCreate(gamesSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for Games setting"));
        return;
    }

    this->m_pConfigSettings = new QSettings(gamesSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfGames = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfGames; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString home      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString away        = this->m_pConfigSettings->value(PLAY_AWAY, "").toString();
            quint8  saisonIndex = quint8(this->m_pConfigSettings->value(PLAY_SAISON_INDEX, 0).toUInt());
            QString score       = this->m_pConfigSettings->value(PLAY_SCORE, "").toString();
            quint8  competition = quint8(this->m_pConfigSettings->value(PLAY_COMPETITION, 0).toUInt());

            if (!this->addNewGamesPlay(home, away, timestamp, saisonIndex, score, competition, index))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems        = true;
        GamesPlay* pGame = (GamesPlay*)(this->getProblemItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        pGame->m_index = this->getNextInternalIndex();
        this->addNewGamesPlay(pGame->m_itemName, pGame->away,
                              pGame->m_timestamp, pGame->saisonIndex,
                              pGame->score, pGame->competition,
                              pGame->m_index);
    }

    if (bProblems)
        this->saveCurrentInteralList();

    this->sortGamesListByTime();
}

int Games::addNewGame(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp)
{
    if (sIndex == 0 || comp == 0) {
        qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
        return ERROR_CODE_COMMON;
    }

    GamesPlay* pGame;
    if ((pGame = this->gameExists(sIndex, comp, timestamp)) != NULL) {
        //        QString info = QString("%1 : %2").arg(sIndex).arg(comp);
        //        qInfo() << (QString("Game \"%1\" already exists, updating info").arg(info));

        QMutexLocker locker(&this->m_mInternalInfoMutex);

        if (pGame->m_itemName != home) {
            if (this->updateItemValue(pGame, ITEM_NAME, QVariant(home)))
                pGame->m_itemName = home;
        }
        if (pGame->away != away) {
            if (this->updateItemValue(pGame, PLAY_AWAY, QVariant(away)))
                pGame->away = away;
        }
        if (pGame->m_timestamp != timestamp) {
            if (this->updateItemValue(pGame, ITEM_TIMESTAMP, QVariant(timestamp))) {
                pGame->m_timestamp = timestamp;
            }
        }
        if (pGame->score != score && score.size() > 0) {
            if (this->updateItemValue(pGame, PLAY_SCORE, QVariant(score)))
                pGame->score = score;
        }

        return pGame->m_index;
    }

    int newIndex = this->getNextInternalIndex();

    QMutexLocker locker(&this->m_mConfigIniMutex);


    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, home);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(PLAY_AWAY, away);
    this->m_pConfigSettings->setValue(PLAY_SAISON_INDEX, sIndex);
    this->m_pConfigSettings->setValue(PLAY_SCORE, score);
    this->m_pConfigSettings->setValue(PLAY_COMPETITION, comp);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->addNewGamesPlay(home, away, timestamp, sIndex, score, comp, newIndex, false);

    CONSOLE_INFO(QString("Added new game: %1").arg(home + " : " + away));
    return newIndex;
}

int Games::showAllGames()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        QString date = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString output;
        if (pGame->score.size() > 0)
            output = QString("%1: %2 - %3 %4 - %5 = %6").arg(pGame->saisonIndex).arg(pGame->competition).arg(date, pGame->m_itemName, pGame->away, pGame->score);
        else
            output = QString("%1: %2 - %3 %4 - %5").arg(pGame->saisonIndex).arg(pGame->competition).arg(date, pGame->m_itemName, pGame->away);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void Games::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;

        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pGame->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pGame->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pGame->m_index);

        this->m_pConfigSettings->setValue(PLAY_AWAY, pGame->away);
        this->m_pConfigSettings->setValue(PLAY_SAISON_INDEX, pGame->saisonIndex);
        this->m_pConfigSettings->setValue(PLAY_SCORE, pGame->score);
        this->m_pConfigSettings->setValue(PLAY_COMPETITION, pGame->competition);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved actual Games List with %1 entries").arg(this->getNumberOfInternalList());
}

GamesPlay* Games::gameExists(quint8 sIndex, quint8 comp, qint64 timestamp)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    QDateTime date = QDateTime::fromMSecsSinceEpoch(timestamp);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        GamesPlay* pGame = (GamesPlay*)(this->getItemFromArrayIndex(i));
        if (pGame == NULL)
            continue;
        if (pGame->saisonIndex == sIndex && pGame->competition == comp) {
            QDateTime oldData = QDateTime::fromMSecsSinceEpoch(pGame->m_timestamp);
            if (date.date().year() == oldData.date().year() && date.date().month() == oldData.date().month())
                return pGame;
        }
    }
    return NULL;
}


//bool ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, bool checkUser)
bool Games::addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, bool checkGame)
{
    if (checkGame) {
        if (sIndex == 0 || comp == 0) {
            qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
            return false;
        }
        if (index == 0 || itemExists(index)) {
            qWarning().noquote() << QString("Game \"%1\" with index \"%2\" already exists, saving with new index").arg(home + " - " + away).arg(index);
            this->addNewGamesPlay(home, away, timestamp, sIndex, score, comp, index, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewGamesPlay(home, away, timestamp, sIndex, score, comp, index, &this->m_lInteralList);
    return true;
}

void Games::addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, QList<ConfigItem*>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    GamesPlay* play   = new GamesPlay();
    play->m_itemName  = home;
    play->away        = away;
    play->m_timestamp = timestamp;
    play->saisonIndex = sIndex;
    play->score       = score;
    play->competition = comp;
    play->m_index     = index;
    pList->append(play);
}


void Games::sortGamesListByTime()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), ConfigItem::compareTimeStampFunction);
}


Games::~Games()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
