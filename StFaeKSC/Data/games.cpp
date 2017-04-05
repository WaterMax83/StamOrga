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

    this->m_pGamesSettings = new QSettings(gamesSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mGamesIniMutex);

        this->m_pGamesSettings->beginGroup(GAMES_GROUP);
        int sizeOfGames = this->m_pGamesSettings->beginReadArray(PLAY_ARRAY);

        for (int i=0; i<sizeOfGames; i++ ) {
            this->m_pGamesSettings->setArrayIndex(i);
            QString home = this->m_pGamesSettings->value(PLAY_HOME, "").toString();
            QString away = this->m_pGamesSettings->value(PLAY_AWAY, "").toString();
            qint64 datetime = this->m_pGamesSettings->value(PLAY_DATETIME, 0x0).toULongLong();
            quint8 saisonIndex = quint8(this->m_pGamesSettings->value(PLAY_SAISON_INDEX, 0).toUInt());
            QString score = this->m_pGamesSettings->value(PLAY_SCORE, "").toString();
            quint8 competition = quint8(this->m_pGamesSettings->value(PLAY_COMPETITION, 0).toUInt());
            quint32 index = this->m_pGamesSettings->value(PLAY_INDEX, 0).toInt();
            if (!this->addNewGamesPlay(home, away, datetime, saisonIndex, score, competition, index))
                bProblems = true;
        }
        this->m_pGamesSettings->endArray();
        this->m_pGamesSettings->endGroup();
    }


    for (int i=0; i<this->m_lAddGamesPlayProblems.size(); i++)
    {
        bProblems = true;
        this->m_lAddGamesPlayProblems[i].index = this->getNextGameIndex();
        this->addNewGamesPlay(this->m_lAddGamesPlayProblems[i].home, this->m_lAddGamesPlayProblems[i].away,
                              this->m_lAddGamesPlayProblems[i].datetime, this->m_lAddGamesPlayProblems[i].saisonIndex,
                              this->m_lAddGamesPlayProblems[i].score, this->m_lAddGamesPlayProblems[i].competition,
                              this->m_lAddGamesPlayProblems[i].index);
    }

    if (bProblems)
        this->saveActualGamesList();

    this->sortGamesListByTime();
}

int Games::addNewGame(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp)
{
    if (sIndex == 0 || comp == 0) {
        qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
        return ERROR_CODE_COMMON;
    }

    GamesPlay *pGame;
    if ((pGame = this->gameExists(sIndex, comp)) != NULL) {
        CONSOLE_INFO (QString("Game \"%1\" already exists, updating info").arg(sIndex + ":" + comp));

        QMutexLocker locker(&this->m_mGamesListMutex);

        if (pGame->home != home) {
            if (this->updateGamesPlayValue(pGame, PLAY_HOME, QVariant(home)))
                pGame->home = home;
        }
        if (pGame->away != away) {
            if (this->updateGamesPlayValue(pGame, PLAY_AWAY, QVariant(away)))
                pGame->away = away;
        }
        if (pGame->datetime != datetime) {
            if (this->updateGamesPlayValue(pGame, PLAY_DATETIME, QVariant(datetime))) {
                pGame->datetime = datetime;
            }
        }
        if (pGame->score != score) {
            if (this->updateGamesPlayValue(pGame, PLAY_SCORE, QVariant(score)))
                pGame->score = score;
        }

        return pGame->index;
    }

    QMutexLocker locker(&this->m_mGamesIniMutex);


    int newIndex = this->getNextGameIndex();
    this->m_pGamesSettings->beginGroup(GAMES_GROUP);
    this->m_pGamesSettings->beginWriteArray(PLAY_ARRAY);
    this->m_pGamesSettings->setArrayIndex(this->getNumberOfGames());
    this->m_pGamesSettings->setValue(PLAY_HOME, home);
    this->m_pGamesSettings->setValue(PLAY_AWAY, away);
    this->m_pGamesSettings->setValue(PLAY_DATETIME, datetime);
    this->m_pGamesSettings->setValue(PLAY_SAISON_INDEX, sIndex);
    this->m_pGamesSettings->setValue(PLAY_SCORE, score);
    this->m_pGamesSettings->setValue(PLAY_COMPETITION, comp);
    this->m_pGamesSettings->setValue(PLAY_INDEX, newIndex);
    this->m_pGamesSettings->endArray();
    this->m_pGamesSettings->endGroup();
    this->m_pGamesSettings->sync();

    this->addNewGamesPlay(home, away, datetime, sIndex, score, comp, newIndex, false);

    CONSOLE_INFO(QString("Added new game: %1").arg(home + " : " + away));
    return newIndex;
}

//int ListedUser::removeUser(const QString &name)
//{
//    int index = this->getUserLoginIndex(name);
//    if (index < 0 || index > this->m_lUserLogin.size() - 1)
//    {
//        CONSOLE_WARNING(QString("Could not find user \"%1\"").arg(name));
//        return -1;
//    }

//    QMutexLocker locker(&this->m_mUserListMutex);

//    this->m_lUserLogin.removeAt(index);

//    this->saveActualUserList();

//    CONSOLE_INFO(QString("removed User \"%1\"").arg(name));
//    return 0;
//}

int Games::showAllGames()
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    foreach (GamesPlay play, this->m_lGamesPlay) {
        QString date = QDateTime::fromMSecsSinceEpoch(play.datetime).toString("dd.MM.yyyy hh:mm");
        QString output;
        if (play.score.size() > 0)
            output = QString("%1: %2 - %3 %4 - %5 = %6").arg(play.saisonIndex).arg(play.competition).arg(date, play.home, play.away, play.score);
        else
            output = QString("%1: %2 - %3 %4 - %5").arg(play.saisonIndex).arg(play.competition).arg(date, play.home, play.away);
        std::cout << output.toStdString() << std::endl;
    }
    return 0;
}

void Games::saveActualGamesList()
{
    QMutexLocker locker(&this->m_mGamesIniMutex);

    this->m_pGamesSettings->beginGroup(GAMES_GROUP);
    this->m_pGamesSettings->remove("");              // clear all elements

    this->m_pGamesSettings->beginWriteArray(PLAY_ARRAY);
    for (int i=0; i<this->m_lGamesPlay.size(); i++) {
        this->m_pGamesSettings->setArrayIndex(i);
        this->m_pGamesSettings->setValue(PLAY_HOME, this->m_lGamesPlay[i].home);
        this->m_pGamesSettings->setValue(PLAY_ARRAY, this->m_lGamesPlay[i].away);
        this->m_pGamesSettings->setValue(PLAY_DATETIME, this->m_lGamesPlay[i].datetime);
        this->m_pGamesSettings->setValue(PLAY_SAISON_INDEX, this->m_lGamesPlay[i].saisonIndex);
        this->m_pGamesSettings->setValue(PLAY_SCORE, this->m_lGamesPlay[i].score);
        this->m_pGamesSettings->setValue(PLAY_COMPETITION, this->m_lGamesPlay[i].competition);
        this->m_pGamesSettings->setValue(PLAY_INDEX, this->m_lGamesPlay[i].index);
    }

    this->m_pGamesSettings->endArray();
    this->m_pGamesSettings->endGroup();

    qDebug().noquote() << QString("saved actual Games List with %1 entries").arg(this->m_lGamesPlay.size());
}

GamesPlay *Games::gameExists(quint8 sIndex, quint8 comp)
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    for (int i=0; i<this->m_lGamesPlay.size(); i++) {
        if (this->m_lGamesPlay[i].saisonIndex == sIndex && this->m_lGamesPlay[i].competition == comp)
            return &this->m_lGamesPlay[i];
    }
    return NULL;
}

bool Games::gameExists(quint32 index)
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    foreach (GamesPlay play, this->m_lGamesPlay) {
        if (play.index == index)
            return true;
    }
    return false;
}

//bool ListedUser::userCheckPassword(QString name, QString passw)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    foreach (UserLogin login, this->m_lUserLogin) {
//        if (login.userName == name) {
//            if (login.password == passw)
//                return true;
//            return false;
//        }
//    }
//    return false;
//}

//bool ListedUser::userChangePassword(QString name, QString passw)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    for (int i=0; i<this->m_lUserLogin.size(); i++) {
//        if (this->m_lUserLogin[i].userName == name) {
//            if (this->updateUserLoginValue(&this->m_lUserLogin[i], LOGIN_PASSWORD, QVariant(passw))) {
//                this->m_lUserLogin[i].password = passw;
//                return true;
//            }
//        }
//    }
//    return false;
//}

//bool ListedUser::userChangeProperties(QString name, quint32 props)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    if (name.length() < MIN_SIZE_USERNAME)
//        return false;

//    for (int i=0; i<this->m_lUserLogin.size(); i++) {
//        if (this->m_lUserLogin[i].userName == name) {
//            if (this->updateUserLoginValue(&this->m_lUserLogin[i], LOGIN_PROPERTIES, QVariant(props))) {
//                this->m_lUserLogin[i].properties = props;
//                return true;
//            } else
//                return false;
//        }
//    }
//    return false;
//}

//quint32 ListedUser::getUserProperties(QString name)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    foreach (UserLogin login, this->m_lUserLogin) {
//        if (login.userName == name)
//            return login.properties;
//    }
//    return 0;
//}

//bool ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, bool checkUser)
bool Games::addNewGamesPlay(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp, quint32 index, bool checkGame)
{
    if (checkGame) {
        if (sIndex == 0 || comp == 0) {
            qWarning().noquote() << "Could not add game because saisonIndex or competition were zero";
            return false;
        }
        if (index == 0 || gameExists(index)) {
            qWarning().noquote() << QString("Game \"%1\" with index \"%2\" already exists, saving with new index").arg(home + " - " + away).arg(index);
            this->addNewGamesPlay(home, away, datetime, sIndex, score, comp, index, &this->m_lAddGamesPlayProblems);
            return false;
        }
    }

    this->addNewGamesPlay(home, away, datetime, sIndex, score, comp, index, &this->m_lGamesPlay);
    return true;
}

void Games::addNewGamesPlay(QString home, QString away, qint64 datetime, quint8 sIndex, QString score, quint8 comp, quint32 index, QList<GamesPlay> *pList)
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    GamesPlay play;
    play.home = home;
    play.away = away;
    play.datetime = datetime;
    play.saisonIndex = sIndex;
    play.score = score;
    play.competition = comp;
    play.index = index;
    pList->append(play);
}

//quint32 ListedUser::getUserLoginIndex(const QString &name)
//{
//    QMutexLocker locker(&this->m_mUserListMutex);

//    for (int i=0; i<this->m_lUserLogin.size(); i++) {
//        if (this->m_lUserLogin[i].userName == name)
//            return i;
//    }
//    return -1;
//}

quint32 Games::getNextGameIndex()
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    quint32 index = 0;
    for (int i=0; i<this->m_lGamesPlay.size(); i++) {
        if (this->m_lGamesPlay[i].index > index)
            index = this->m_lGamesPlay[i].index;
    }
    return index+1;
}

bool Games::updateGamesPlayValue(GamesPlay *pGame, QString key, QVariant value)
{
    bool rValue = false;
    QMutexLocker locker(&this->m_mGamesIniMutex);

    this->m_pGamesSettings->beginGroup(GAMES_GROUP);
    int arrayCount = this->m_pGamesSettings->beginReadArray(PLAY_ARRAY);
    for (int i=0; i<arrayCount; i++) {
        this->m_pGamesSettings->setArrayIndex(i);
        quint32 actIndex = this->m_pGamesSettings->value(PLAY_INDEX, 0).toInt();
        if (pGame->index == actIndex) {

            this->m_pGamesSettings->setValue(key, value);
            qInfo().noquote() << QString("Change %1 of game %2:%3 to %4").arg(key, pGame->saisonIndex, pGame->competition).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pGamesSettings->endArray();
    this->m_pGamesSettings->endGroup();
    return rValue;
}

void Games::sortGamesListByTime()
{
    QMutexLocker locker(&this->m_mGamesListMutex);

    std::sort(this->m_lGamesPlay.begin(), this->m_lGamesPlay.end(), GamesPlay::compareDateTimeFunction);
}


Games::~Games()
{
    if (this->m_pGamesSettings != NULL)
        delete this->m_pGamesSettings;
}
