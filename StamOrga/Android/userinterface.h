#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>

#include "../connectionhandling.h"

class UserInterface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName READ userName WRITE setUserName)
    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr)
    Q_PROPERTY(quint16 port READ port WRITE setPort)
public:
    explicit UserInterface(QObject *parent = 0);

    Q_INVOKABLE void StartSendingData();


    QString userName() { return this->m_UserName; }
    void setUserName(const QString &s) { this->m_UserName = s; }

    QString ipAddr() { return this->m_ipAddr; }
    void setIpAddr(const QString &s) { this->m_ipAddr = s; }

    quint16 port() { return this->m_port; }
    void setPort(const quint16 &p) { this->m_port = p; }

signals:
    void notifyConnectionFinished();

public slots:
    void ConnectionFinished();


private:
    QString m_UserName;
    QString m_ipAddr;
    quint16 m_port;

    ConnectionHandling *m_pMainCon;
};

#endif // USERINTERFACE_H
