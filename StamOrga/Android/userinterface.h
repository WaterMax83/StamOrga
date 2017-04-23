#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QtCore/QObject>

#include "../connectionhandling.h"
#include "../Data/globaldata.h"
#include "../../Common/General/globalfunctions.h"

class UserInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(GlobalData * globalData READ globalData WRITE setGlobalData)
public:
    explicit UserInterface(QObject *parent = 0);

    Q_INVOKABLE qint32 startMainConnection(QString name, QString passw);

    Q_INVOKABLE qint32 startGettingGamesList();

    Q_INVOKABLE qint32 startUpdateUserPassword(QString newPassw);

    Q_INVOKABLE qint32 startUpdateReadableName(QString name);

    Q_INVOKABLE qint32 startAddSeasonTicket(QString name, quint32 discount);

    Q_INVOKABLE qint32 startRemoveSeasonTicket(quint32 index);

    Q_INVOKABLE qint32 startGettingSeasonTicketList();

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

    Q_INVOKABLE QString getErrorCodeToString(qint32 code)
    {
        return getErrorCodeString(code);
    }

    GlobalData *globalData()
    {
        return this->m_pConHandle->getGlobalData();
    }
    void setGlobalData(GlobalData *pData)
    {
        this->m_pConHandle->setGlobalData(pData);
    }

signals:
    void notifyConnectionFinished(qint32 result);
    void notifyVersionRequestFinished(qint32 result, QString msg);
    void notifyUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void notifyUpdateReadableNameRequest(qint32 result);
    void notifyGamesListFinished(qint32 result);
    void notifySeasonTicketAddFinished(qint32 result);
    void notifySeasonTicketRemoveFinished(qint32 result);
    void notifySeasonTicketListFinished(qint32 result);

public slots:
    void slConnectionRequestFinished(qint32 result);
    void slVersionRequestFinished(qint32 result, QString msg);
    void slUpdatePasswordRequestFinished(qint32 result, QString newPassWord);
    void slUpdateReadableNameRequestFinished(qint32 result);
    void slGettingGamesListFinished(qint32 result);
    void slSeasonTicketAddFinished(qint32 result);
    void slSeasonTicketRemoveFinished(qint32 result);
    void slSeasonTicketListFinished(qint32 result);


private:
    ConnectionHandling *m_pConHandle;
};

#endif // USERINTERFACE_H
