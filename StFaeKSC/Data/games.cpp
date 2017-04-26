/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>

#include <iostream>

#include "games.h"
#include "../Common/General/globalfunctions.h"

Games::Games()
{
    QString gamesSetFilePath = getUserHomeConfigPath() + "/Settings/Games.ini";

    if (!checkFilePathExistAndCreate(gamesSetFilePath))
    {
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

        for (int i=0; i<sizeOfGames; i++ ) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString home = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            qint64 timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();
            quint32 index = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();

            QString away = this->m_pConfigSettings->value(PLAY_AWAY, "").toString();
            quint8 saisonIndex = quint8(this->m_pConfigSettings->value(PLAY_SAISON_INDEX, 0).toUInt());
            QString score = this->m_pConfigSettings->value(PLAY_SCORE, "").toString();
            quint8 competition = quint8(this->m_pConfigSettings->value(PLAY_COMPETITION, 0).toUInt());

            if (!this->addNewGamesPlay(home, away, timestamp, saisonIndex, score, competition, index))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i=0; i<this->m_lAddItemProblems.size(); i++)
    {
        bProblems = true;
        this->m_lAddItemProblems[i].m_index = this->getNextInternalIndex();
        this->addNewGamesPlay(this->m_lAddItemProblems[i].m_itemName, this->m_lAddItemProblems[i].away,
                              this->m_lAddItemProblems[i].m_timestamp, this->m_lAddItemProblems[i].saisonIndex,
                              this->m_lAddItemProblems[i].score, this->m_lAddItemProblems[i].competition,
                              this->m_lAddItemProblems[i].m_index);
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

    GamesPlay *pGame;
    if ((pGame = this->gameExists(sIndex, comp, timestamp)) != NULL) {
//        QString info = QString("%1 : %2").arg(sIndex).arg(comp);
//        qInfo() << (QString("Game \"%1\" already exists, updating info").arg(info));

        QMutexLocker locker(&this->m_mInternalInfoMutex);

        if (pGame->m_itemName != home) {
            if (this->updateGamesPlayValue(pGame, ITEM_NAME, QVariant(home)))
                pGame->m_itemName = home;
        }
        if (pGame->away != away) {
            if (this->updateGamesPlayValue(pGame, PLAY_AWAY, QVariant(away)))
                pGame->away = away;
        }
        if (pGame->m_timestamp != timestamp) {
            if (this->updateGamesPlayValue(pGame, ITEM_TIMESTAMP, QVariant(timestamp))) {
                pGame->m_timestamp = timestamp;
            }
        }
        if (pGame->score != score && score.size() > 0) {
            if (this->updateGamesPlayValue(pGame, PLAY_SCORE, QVariant(score)))
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

    foreach (GamesPlay play, this->m_lInteralList) {
        QString date = QDateTime::fromMSecsSinceEpoch(play.m_timestamp).toString("dd.MM.yyyy hh:mm");
        QString output;
        if (play.score.size() > 0)
            output = QString("%1: %2 - %3 %4 - %5 = %6").arg(play.saisonIndex).arg(play.competition).arg(date, play.m_itemName, play.away, play.score);
        else
            output = QString("%1: %2 - %3 %4 - %5").arg(play.saisonIndex).arg(play.competition).arg(date, play.m_itemName, play.away);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void Games::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove("");              // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i=0; i<this->m_lInteralList.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, this->m_lInteralList[i].m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, this->m_lInteralList[i].m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, this->m_lInteralList[i].m_index);

        this->m_pConfigSettings->setValue(PLAY_AWAY, this->m_lInteralList[i].away);
        this->m_pConfigSettings->setValue(PLAY_SAISON_INDEX, this->m_lInteralList[i].saisonIndex);
        this->m_pConfigSettings->setValue(PLAY_SCORE, this->m_lInteralList[i].score);
        this->m_pConfigSettings->setValue(PLAY_COMPETITION, this->m_lInteralList[i].competition);

    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved actual Games List with %1 entries").arg(this->m_lInteralList.size());
}

GamesPlay *Games::gameExists(quint8 sIndex, quint8 comp, qint64 timestamp)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    QDateTime date = QDateTime::fromMSecsSinceEpoch(timestamp);
    for (int i=0; i<this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].saisonIndex == sIndex && this->m_lInteralList[i].competition == comp) {
            QDateTime oldData = QDateTime::fromMSecsSinceEpoch(this->m_lInteralList[i].m_timestamp);
            if (date.date().year() == oldData.date().year() &&
                date.date().month() == oldData.date().month())
                return &this->m_lInteralList[i];
        }
    }
    return NULL;
}

bool Games::gameExists(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (GamesPlay play, this->m_lInteralList) {
        if (play.m_index == index)
            return true;
    }
    return false;
}


//bool ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, bool checkUser)
bool Games::addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, bool checkGame)
{
    if (checkGame) {
        if (sIndex == 0 || comp == 0) {
            qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
            return false;
        }
        if (index == 0 || gameExists(index)) {
            qWarning().noquote() << QString("Game \"%1\" with index \"%2\" already exists, saving with new index").arg(home + " - " + away).arg(index);
            this->addNewGamesPlay(home, away, timestamp, sIndex, score, comp, index, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewGamesPlay(home, away, timestamp, sIndex, score, comp, index, &this->m_lInteralList);
    return true;
}

void Games::addNewGamesPlay(QString home, QString away, qint64 timestamp, quint8 sIndex, QString score, quint8 comp, quint32 index, QList<GamesPlay> *pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    GamesPlay play;
    play.m_itemName = home;
    play.away = away;
    play.m_timestamp = timestamp;
    play.saisonIndex = sIndex;
    play.score = score;
    play.competition = comp;
    play.m_index = index;
    pList->append(play);
}


bool Games::updateGamesPlayValue(GamesPlay *pGame, QString key, QVariant value)
{
    bool rValue = false;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int arrayCount = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    for (int i=0; i<arrayCount; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        quint32 actIndex = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
        if (pGame->m_index == actIndex) {

            this->m_pConfigSettings->setValue(key, value);
            qInfo().noquote() << QString("Change %1 of game %2-%3 to %4").arg(key).arg(pGame->saisonIndex, pGame->competition).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    return rValue;
}

void Games::sortGamesListByTime()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), GamesPlay::compareTimeStampFunction);
}


Games::~Games()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
