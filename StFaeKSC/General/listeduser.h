#ifndef LISTEDUSER_H
#define LISTEDUSER_H


#include <QtCore/QSettings>
#include <QtCore/QList>

struct UserLogin {
    QString userName;
    QString password;
    quint32 properties;
    quint32 index;
};

class ListedUser
{
public:
    ListedUser();
    ~ListedUser();

    bool addNewUser(const QString &name);

    quint32 getNumberOfUsers() { return this->m_lUserLogin.size(); }

    bool userExists(QString name);
    bool userExists(quint32 index);

private:
    QSettings           *m_pUserSettings = NULL;
    QList<UserLogin>    m_lUserLogin;

    void addNewUserLogin(QString name, QString password, quint32 prop, quint32 index);

};

#endif // LISTEDUSER_H
