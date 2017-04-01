#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QtCore/QObject>

#include "../connectionhandling.h"
#include "../globaldata.h"

class UserInterface : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString userName READ userName WRITE setUserName)
//    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr)
//    Q_PROPERTY(quint16 port READ port WRITE setPort)
      Q_PROPERTY(GlobalData * globalData READ globalData WRITE setGlobalData)
public:
    explicit UserInterface(QObject *parent = 0);

    Q_INVOKABLE qint32 startSendingData(QString name, QString passw);


//    QString userName() { return this->m_UserName; }
//    void setUserName(const QString &s) { this->m_UserName = s; }

//    QString ipAddr() { return this->m_ipAddr; }
//    void setIpAddr(const QString &s) { this->m_ipAddr = s; }

    GlobalData *globalData() { return this->m_pGlobalData; }
    void setGlobalData(GlobalData *pData) { this->m_pGlobalData = pData; }

signals:
    void notifyConnectionFinished(qint32 result);
    void notifyVersionRequestFinished(qint32 result, QString msg);

public slots:
    void slConnectionRequestFinished(qint32 result);
    void slVersionRequestFinished(qint32 result, QString msg);


private:
//    QString m_UserName;
//    QString m_ipAddr;
//    quint16 m_port;

    ConnectionHandling *m_pConHandle;

    GlobalData *m_pGlobalData = NULL;
};

#endif // USERINTERFACE_H
