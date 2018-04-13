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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSettings>
//#include <QtNetwork/QHostInfo>

#include "../../Common/General/backgroundcontroller.h"
#include "../../Common/General/globalfunctions.h"
#include "../../Common/General/logging.h"
//#include "cdatagameuserdata.h"
//#include "cdatameetinginfo.h"
//#include "gameplay.h"
//#include "newsdataitem.h"
//#include "seasonticket.h"
//#include "source/cadrpushnotifyinfohandler.h"


//#define USER_IS_ENABLED(val) ((this->m_UserProperties & val) > 0 ? true : false)

class GlobalData : public QObject
{
    Q_OBJECT
    //    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    //    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged)
    //    Q_PROPERTY(QString readableName READ readableName WRITE setReadableName NOTIFY readableNameChanged)
    //    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr NOTIFY ipAddrChanged)
    //    Q_PROPERTY(quint32 conMasterPort READ conMasterPort WRITE setConMasterPort NOTIFY conMasterPortChanged)
    //    Q_PROPERTY(bool bIsConnected READ bIsConnected WRITE setbIsConnected NOTIFY bIsConnectedChanged)

    friend class GlobalSettings;

public:
    explicit GlobalData(QObject* parent = 0);

    //    qint32 initialize() { return ERROR_CODE_SUCCESS; }

    //    void loadGlobalSettings();

    //    QString userName();
    //    void    setUserName(const QString& user);

    //    QString getSalt() { return this->m_salt; }
    //    void    setSalt(QString salt) { this->m_salt = salt; }

    //    QString readableName();
    //    void    setReadableName(const QString& name);

    //    QString passWord();
    //    void    setPassWord(const QString& passw);

    //    QString ipAddr();
    //    void    setIpAddr(const QString& ip);

    //    quint32 conMasterPort();
    //    void    setConMasterPort(quint32 port);

    //    quint32 conDataPort();
    //    void    setConDataPort(quint32 port);

    //    Q_INVOKABLE quint32 userIndex();
    //    void                setUserIndex(quint32 userIndex);

    //    bool bIsConnected() { return this->m_bIsConnected; }
    //    void setbIsConnected(bool enable)
    //    {
    //        if (this->m_bIsConnected != enable) {
    //            this->m_bIsConnected = enable;
    //            emit this->bIsConnectedChanged();
    //        }
    //    }

    //    Q_INVOKABLE bool userIsDebugEnabled();
    //    Q_INVOKABLE bool userIsGameAddingEnabled();
    //    Q_INVOKABLE bool userIsGameFixedTimeEnabled();
    //    Q_INVOKABLE bool userIsFanclubEnabled();
    //    Q_INVOKABLE bool userIsFanclubEditEnabled();
    //    void             SetUserProperties(quint32 value);
    //    quint32          getUserProperties();

    //    void saveGlobalUserSettings();

    //    Q_INVOKABLE QString getCurrentLoggingList(int index);

    //    Q_INVOKABLE QStringList getCurrentLogFileList();

    //    Q_INVOKABLE void deleteCurrentLoggingFile(int index);

    //    Q_INVOKABLE void copyTextToClipBoard(QString text);

    //    void saveCurrentGamesList(qint64 timestamp);

    //    void        startUpdateGamesPlay(const qint16 updateIndex);
    //    void        addNewGamePlay(GamePlay* gPlay, const qint16 updateIndex = 0);
    //    Q_INVOKABLE quint32 getGamePlayLength()
    //    {
    //        QMutexLocker lock(&this->m_mutexGame);
    //        return this->m_lGamePlay.size();
    //    }
    //    GamePlay*   getGamePlay(const quint32 gameIndex);
    //    Q_INVOKABLE GamePlay* getGamePlayFromArrayIndex(int index);
    //    Q_INVOKABLE QString getGamePlayLastUpdateString();
    //    qint64              getGamePlayLastLocalUpdate();
    //    qint64              getGamePlayLastServerUpdate();
    //    void                resetAllGamePlayEvents();
    //    bool                setGamePlayItemHasEvent(quint32 gameIndex);


    //    void saveCurrentSeasonTickets(qint64 timestamp);

    //    void        startUpdateSeasonTickets(const quint16 updateIndex);
    //    void        addNewSeasonTicket(SeasonTicketItem* sTicket, const quint16 updateIndex = 0);
    //    Q_INVOKABLE quint32 getSeasonTicketLength()
    //    {
    //        QMutexLocker lock(&this->m_mutexTicket);
    //        return this->m_lSeasonTicket.size();
    //    }
    //    Q_INVOKABLE SeasonTicketItem* getSeasonTicketFromArrayIndex(int index);
    //    SeasonTicketItem*             getSeasonTicket(qint32 ticketIndex);
    //    Q_INVOKABLE QString getSeasonTicketLastLocalUpdateString();
    //    qint64              getSeasonTicketLastLocalUpdate();
    //    qint64              getSeasonTicketLastServerUpdate();
    //    void                resetSeasonTicketLastServerUpdate();

    //    Q_INVOKABLE cDataMeetingInfo* getMeetingInfo(quint32 type)
    //    {
    //        if (type == MEETING_TYPE_MEETING)
    //            return &this->m_meetingInfo;
    //        else
    //            return &this->m_awayTripInfo;
    //    }


    //    void          saveCurrentNewsDataList(qint64 timestamp);
    //    void          startUpdateNewsDataItem(const quint16 updateIndex);
    //    void          addNewNewsDataItem(NewsDataItem* pItem, const quint16 updateIndex = 0);
    //    NewsDataItem* createNewNewsDataItem(quint32 newsIndex, QString header, QString info);
    //    Q_INVOKABLE quint32 getNewsDataItemLength()
    //    {
    //        QMutexLocker lock(&this->m_mutexNewsData);
    //        return this->m_lNewsDataItems.size();
    //    }
    //    Q_INVOKABLE NewsDataItem* getNewsDataItemFromArrayIndex(int index);
    //    NewsDataItem*             getNewsDataItem(quint32 newsIndex);
    //    Q_INVOKABLE QString getNewsDataLastLocalUpdateString();
    //    qint64              getNewsDataLastLocalUpdate();
    //    qint64              getNewsDataLastServerUpdate();
    //    void                resetNewsDataLastServerUpdate();
    //    void                resetAllNewsDataEvents();
    //    bool                setNewsDataItemHasEvent(quint32 newsIndex);

    //    Q_INVOKABLE GameUserData* getGameUserDataHandler() { return &this->m_gameUserData; }

    //    QString getCurrentAppGUID() { return this->m_AppInstanceGUID; }
    //    QString getCurrentAppToken() { return this->m_pushNotificationToken; }

    //    bool isIPLookUpDone() { return this->m_bIpAdressWasSet; }

    //    void setUpdateLink(QString link) { this->m_updateLink = link; }
    //    Q_INVOKABLE QString getUpdateLink() { return this->m_updateLink; }

signals:
    //    void
    //    userNameChanged();
    //    void passWordChanged();
    //    void readableNameChanged();
    //    void ipAddrChanged();
    //    void conMasterPortChanged();
    //    void bIsConnectedChanged();


public slots:

private slots:
//    void callBackLookUpHost(const QHostInfo& host);
#ifdef Q_OS_ANDROID
//    void slotNewFcmRegistrationToken(QString token);
#endif

private:
    //    QString m_userName;
    //    QString m_passWord;
    //    QString m_salt;
    //    QString m_readableName;
    //    QString m_ipAddress;
    //    quint32 m_uMasterPort;
    //    quint16 m_uDataPort;
    //    quint32 m_userIndex;
    //    bool    m_bIpAdressWasSet;

    //    quint32 m_UserProperties;

    //    QMutex m_mutexUser;
    //    QMutex m_mutexUserIni;
    //    QMutex m_mutexGame;
    //    QMutex m_mutexTicket;
    //    QMutex m_mutexNewsData;

    //    bool m_bIsConnected;

    QSettings* m_pMainUserSettings;

    //    QList<GamePlay*> m_lGamePlay;
    //    qint64           m_gpLastLocalUpdateTimeStamp;
    //    qint64           m_gpLastServerUpdateTimeStamp;
    //    bool             m_bGamePlayLastUpdateDidChanges;

    //    QList<SeasonTicketItem*> m_lSeasonTicket;
    //    qint64                   m_stLastLocalUpdateTimeStamp;
    //    qint64                   m_stLastServerUpdateTimeStamp;
    //    bool                     m_bSeasonTicketLastUpdateDidChanges;

    //    cDataMeetingInfo m_meetingInfo;
    //    cDataMeetingInfo m_awayTripInfo;

    //    QList<NewsDataItem*> m_lNewsDataItems;
    //    qint64               m_ndLastLocalUpdateTimeStamp;
    //    qint64               m_ndLastServerUpdateTimeStamp;

    //    AdrPushNotifyInfoHandler* m_pushNotificationInfoHandler;
    //    QMutex                              m_pushNotificationMutex;
    //    QString                             m_pushNotificationToken;
    //    QString                             m_AppInstanceGUID;

    //    QString m_updateLink;

    //    GameUserData m_gameUserData;

    //    Logging*             m_logApp;
    //    BackgroundController m_ctrlLog;
};

#endif // GLOBALDATA_H
