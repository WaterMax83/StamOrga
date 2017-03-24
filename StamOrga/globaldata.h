#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

class GlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr NOTIFY ipAddrChanged)
    Q_PROPERTY(quint32 conPort READ conPort WRITE setConPort NOTIFY conPortChanged)
public:
    explicit GlobalData(QObject *parent = 0);

    QString userName() { return this->m_userName; }
    void setUserName(const QString &user)
    {
        if (this->m_userName != user) {
            this->m_userName = user;
            emit userNameChanged();
        }
    }

    QString ipAddr() { return this->m_ipAddress; }
    void setIpAddr(const QString &ip)
    {
        if (this->m_ipAddress != ip) {
            this->m_ipAddress = ip;
            emit ipAddrChanged();
        }
    }

    quint32 conPort() { return this->m_uPort; }
    void setConPort(quint32 port)
    {
        if (this->m_uPort != port) {
            this->m_uPort = port;
            emit conPortChanged();
        }
    }

    void saveGlobalUserSettings();

signals:
    void userNameChanged();
    void ipAddrChanged();
    void conPortChanged();

public slots:

private:
    QString m_userName;
    QString m_ipAddress;
    quint32 m_uPort;

    QSettings *m_pMainUserSettings;
};

#endif // GLOBALDATA_H
