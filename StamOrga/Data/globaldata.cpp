#include <QtGui/QGuiApplication>
#include <QtCore/QDebug>


#include "globaldata.h"

#define GAMES_GROUP             "GAMES_LIST"
#define PLAY_ARRAY              "play"
#define PLAY_HOME               "home"
#define PLAY_AWAY               "away"
#define PLAY_DATETIME           "datetime"
#define PLAY_SAISON_INDEX       "sIndex"
#define PLAY_INDEX              "index"
#define PLAY_SCORE              "score"
#define PLAY_COMPETITION        "competition"

GlobalData::GlobalData(QObject *parent) : QObject(parent)
{
    QGuiApplication::setOrganizationName("WaterMax");
    QGuiApplication::setApplicationName("StamOrga");
    this->setbIsConnected(false);
    this->uUserProperties = 0x0;

    QHostInfo::lookupHost("watermax83.ddns.net", this, SLOT(callBackLookUpHost(QHostInfo)));
}

void GlobalData::loadGlobalSettings()
{
//    QMutexLocker lock(&this->m_mutexUserIni);

    this->m_pMainUserSettings = new QSettings();

    this->m_pMainUserSettings->beginGroup("USER_LOGIN");

    this->setUserName(this->m_pMainUserSettings->value("UserName", "").toString());
    this->setPassWord(this->m_pMainUserSettings->value("Password", "").toString());
    this->setReadableName(this->m_pMainUserSettings->value("ReadableName", "").toString());
    this->setIpAddr(this->m_pMainUserSettings->value("IPAddress", "140.80.61.57").toString());
    this->setConMasterPort(this->m_pMainUserSettings->value("ConMasterPort", 55000).toInt());

    this->m_pMainUserSettings->endGroup();

    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);

    this->m_gpLastTimeStamp = this->m_pMainUserSettings->value("TIMESTAMP", 0).toLongLong();

    int count = this->m_pMainUserSettings->beginReadArray(PLAY_ARRAY);
    for (int i=0; i<count; i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        GamePlay *play = new GamePlay();
        play->setHome(this->m_pMainUserSettings->value(PLAY_HOME, "").toString());
        play->setAway(this->m_pMainUserSettings->value(PLAY_ARRAY, "").toString());
        play->setTimeStamp(this->m_pMainUserSettings->value(PLAY_DATETIME, 0).toLongLong());
        play->setIndex(quint8(this->m_pMainUserSettings->value(PLAY_SAISON_INDEX, 0).toUInt()));
        play->setScore(this->m_pMainUserSettings->value(PLAY_SCORE, "").toString());
        play->setCompetition(quint8(this->m_pMainUserSettings->value(PLAY_COMPETITION, 0).toUInt()));
        this->addNewGamePlay(play);
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
    QMutexLocker lock (&this->m_mutexGame);

    this->m_pMainUserSettings->beginGroup(GAMES_GROUP);
    this->m_pMainUserSettings->remove("");              // clear all elements

    this->m_pMainUserSettings->setValue("TIMESTAMP", this->m_gpLastTimeStamp);

    this->m_pMainUserSettings->beginWriteArray(PLAY_ARRAY);
    for (int i=0; i<this->m_lGamePlay.size(); i++) {
        this->m_pMainUserSettings->setArrayIndex(i);
        this->m_pMainUserSettings->setValue(PLAY_HOME, this->m_lGamePlay[i]->home());
        this->m_pMainUserSettings->setValue(PLAY_ARRAY, this->m_lGamePlay[i]->away());
        this->m_pMainUserSettings->setValue(PLAY_DATETIME, this->m_lGamePlay[i]->timestamp());
        this->m_pMainUserSettings->setValue(PLAY_SAISON_INDEX, this->m_lGamePlay[i]->index());
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
    for (int i=0; i<this->m_lGamePlay.size(); i++)
        delete this->m_lGamePlay[i];
    this->m_lGamePlay.clear();
    this->m_gpLastTimeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
}

void GlobalData::addNewGamePlay(GamePlay *gPlay)
{
    if (!this->existGamePlay(gPlay)) {
        QMutexLocker lock(&this->m_mutexGame);
//        qDebug() << QString("Add new game play %1:%2 = %3").arg(gPlay->home(), gPlay->away(), gPlay->score());
        this->m_lGamePlay.append(gPlay);
    }
}

bool GlobalData::existGamePlay(GamePlay *gPlay)
{
    QMutexLocker lock(&this->m_mutexGame);

    if (gPlay->index() == 0 || gPlay->competition() == 0)
        return false;

    for(int i=0; i < this->m_lGamePlay.size(); i++) {
        if (this->m_lGamePlay[i]->index() == gPlay->index() &&
            this->m_lGamePlay[i]->competition() == gPlay->competition() &&
            this->m_lGamePlay[i]->timestamp() == gPlay->timestamp())
            return true;
    }
    return false;
}

GamePlay *GlobalData::getGamePlay(int index)
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

void GlobalData::callBackLookUpHost(const QHostInfo &host)
{
#ifdef Q_OS_ANDROID
    if (host.addresses().size() > 0)
        this->setIpAddr(host.addresses().value(0).toString());
#else
    if (host.addresses().size() > 0)
        qDebug().noquote() << QString("Getting host info ip: %1").arg(host.addresses().value(0).toString()) ;
#endif

}
