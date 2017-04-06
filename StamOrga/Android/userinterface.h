#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QtCore/QObject>

#include "../connectionhandling.h"
#include "../Data/globaldata.h"

class UserInterface : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString userName READ userName WRITE setUserName)
//    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr)
//    Q_PROPERTY(quint16 port READ port WRITE setPort)
      Q_PROPERTY(GlobalData * globalData READ globalData WRITE setGlobalData)
public:
    explicit UserInterface(QObject *parent = 0);

    Q_INVOKABLE qint32 startMainConnection(QString name, QString passw);

    Q_INVOKABLE qint32 startGettingGamesList();

    Q_INVOKABLE qint32 startUpdateUserPassword(QString newPassw);

    Q_INVOKABLE qint32 startUpdateReadableName(QString name);

    Q_INVOKABLE bool isDebuggingEnabled()
    {
#ifdef QT_DEBUG
        return true;
#else
        return false;
#endif
    }

    Q_INVOKABLE bool isDeviceMobile()
    {
#ifdef Q_OS_ANDROID
        return true;
#else
        return false;
#endif
    }

    GlobalData *globalData() { return this->m_pGlobalData; }
    void setGlobalData(GlobalData *pData) { this->m_pGlobalData = pData; }

signals:
    void notifyConnectionFinished(qint32 result);
    void notifyVersionRequestFinished(qint32 result, QString msg);
    void notifyUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void notifyUpdateReadableNameRequest(qint32 result);
    void notifyGamesListFinished(qint32 result);

public slots:
    void slConnectionRequestFinished(qint32 result);
    void slVersionRequestFinished(qint32 result, QString msg);
    void slUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void slUpdateReadableNameRequestFinished(qint32 result);
    void slGettingGamesListFinished(qint32 result);


private:
//    QString m_UserName;
//    QString m_ipAddr;
//    quint16 m_port;

    ConnectionHandling *m_pConHandle;

    GlobalData *m_pGlobalData = NULL;
};

#endif // USERINTERFACE_H
