#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QMutex>
#include <QtCore/QList>
#include <QtNetwork/QHostInfo>

#include "gameplay.h"

class GlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged)
    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr NOTIFY ipAddrChanged)
    Q_PROPERTY(quint32 conMasterPort READ conMasterPort WRITE setConMasterPort NOTIFY conMasterPortChanged)
    Q_PROPERTY(bool bIsConnected READ bIsConnected WRITE setbIsConnected NOTIFY bIsConnectedChanged)

public:
    explicit GlobalData(QObject *parent = 0);

    void loadGlobalSettings();

    QString userName() { QMutexLocker lock(&this->m_mutexUser); return this->m_userName; }
    void setUserName(const QString &user)
    {
        if (this->m_userName != user) {
            {
                QMutexLocker lock(&this->m_mutexUser);
                this->m_userName = user;
            }
            emit userNameChanged();
        }
    }

    QString passWord() { QMutexLocker lock(&this->m_mutexUser); return this->m_passWord; }
    void setPassWord(const QString &passw)
    {
        if (this->m_passWord != passw) {
            {
                QMutexLocker lock(&this->m_mutexUser);
                this->m_passWord = passw;
            }
            emit passWordChanged();
        }
    }

    QString ipAddr() { QMutexLocker lock(&this->m_mutexUser); return this->m_ipAddress; }
    void setIpAddr(const QString &ip)
    {
        if (this->m_ipAddress != ip) {
            {
                QMutexLocker lock(&this->m_mutexUser);
                this->m_ipAddress = ip;
            }
            emit ipAddrChanged();
        }
    }

    quint32 conMasterPort() { QMutexLocker lock(&this->m_mutexUser); return this->m_uMasterPort; }
    void setConMasterPort(quint32 port)
    {
        if (this->m_uMasterPort != port) {
            {
                QMutexLocker lock(&this->m_mutexUser);
                this->m_uMasterPort = port;
            }
            emit conMasterPortChanged();
        }
    }

    quint32 conDataPort() { QMutexLocker lock(&this->m_mutexUser); return this->m_uDataPort; }
    void setConDataPort(quint32 port)
    {
        QMutexLocker lock(&this->m_mutexUser);
        if (this->m_uDataPort != port) {
            this->m_uDataPort = port;
        }
    }

    bool bIsConnected() { return this->m_bIsConnected; }
    void setbIsConnected(bool enable)
    {
        if (this->m_bIsConnected != enable) {
            this->m_bIsConnected = enable;
            emit this->bIsConnectedChanged();
        }
    }
    quint32 uUserProperties;

    void saveGlobalUserSettings();

    void addNewGamePlay(GamePlay *gPlay);
    Q_INVOKABLE quint32 getNumbOfGamePlay() { return this->m_lGamePlay.size(); }
    Q_INVOKABLE GamePlay *getGamePlay(int index)
    {
        if (index < this->m_lGamePlay.size())
            return this->m_lGamePlay.at(index);
        return NULL;
    }

signals:
    void userNameChanged();
    void passWordChanged();
    void ipAddrChanged();
    void conMasterPortChanged();
    void bIsConnectedChanged();

public slots:

private slots:
    void callBackLookUpHost(const QHostInfo &host);

private:
    QString m_userName;
    QString m_passWord;
    QString m_ipAddress;
    quint32 m_uMasterPort;

    QMutex  m_mutexUser;
    QMutex  m_mutexGame;

    bool m_bIsConnected;

    quint16 m_uDataPort;

    QSettings *m_pMainUserSettings;

    QList<GamePlay*> m_lGamePlay;
    bool existGamePlay(GamePlay *gPlay);
};

#endif // GLOBALDATA_H
