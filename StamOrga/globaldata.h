#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

class GlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged)
    Q_PROPERTY(QString ipAddr READ ipAddr WRITE setIpAddr NOTIFY ipAddrChanged)
    Q_PROPERTY(quint32 conMasterPort READ conMasterPort WRITE setConMasterPort NOTIFY conMasterPortChanged)
public:
    explicit GlobalData(QObject *parent = 0);

    void loadGlobalSettings();

    QString userName() { return this->m_userName; }
    void setUserName(const QString &user)
    {
        if (this->m_userName != user) {
            this->m_userName = user;
            emit userNameChanged();
        }
    }

    QString passWord() { return this->m_passWord; }
    void setPassWord(const QString &passw)
    {
        if (this->m_passWord != passw) {
            this->m_passWord = passw;
            emit passWordChanged();
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

    quint32 conMasterPort() { return this->m_uMasterPort; }
    void setConMasterPort(quint32 port)
    {
        if (this->m_uMasterPort != port) {
            this->m_uMasterPort = port;
            emit conMasterPortChanged();
        }
    }

    quint32 conDataPort() { return this->m_uDataPort; }
    void setConDataPort(quint32 port)
    {
        if (this->m_uDataPort != port) {
            this->m_uDataPort = port;
        }
    }

    bool bIsConnected;

    void saveGlobalUserSettings();

signals:
    void userNameChanged();
    void passWordChanged();
    void ipAddrChanged();
    void conMasterPortChanged();

public slots:

private:
    QString m_userName;
    QString m_passWord;
    QString m_ipAddress;
    quint32 m_uMasterPort;

    quint16 m_uDataPort;

    QSettings *m_pMainUserSettings;
};

#endif // GLOBALDATA_H
