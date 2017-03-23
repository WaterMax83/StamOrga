#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <QString>
#include <QMutex>

class ConnectionInfo
{
public:
    ConnectionInfo();

    void SetUserName(const QString &name) { this->m_sUserName = name; }
    QString GetUserName() { return this->m_sUserName; }

    void SetHostAddress(const QString &addr) { this->m_sHostAddress = addr; }
    QString GetHostAddress() { return this->m_sHostAddress; }

    void SetPort(const quint32 port) { this->m_iPort = port; }
    quint16 GetPort() { return this->m_iPort; }

    QMutex m_infoMutex;

private:
    QString m_sUserName;
    QString m_sHostAddress;
    quint16 m_iPort;

};

#endif // CONNECTIONINFO_H
