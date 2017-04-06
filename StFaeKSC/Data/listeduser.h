#ifndef LISTEDUSER_H
#define LISTEDUSER_H


#include <QtCore/QSettings>
#include <QtCore/QList>
#include <QtCore/QMutex>

struct UserLogin {
    QString userName;
    QString password;
    QString readName;
    quint32 properties;
    quint32 index;
};

#define USER_GROUP              "USERS"
#define LOGIN_ARRAY             "logins"
#define LOGIN_USERNAME          "username"
#define LOGIN_PASSWORD          "password"
#define LOGIN_READNAME          "readname"
#define LOGIN_PROPERTIES        "properties"
#define LOGIN_INDEX             "index"

#define DEFAULT_LOGIN_PROPS     0x0

#define MIN_SIZE_USERNAME       5

class ListedUser
{
public:
    ListedUser();
    ~ListedUser();

    int addNewUser(const QString &name, const QString &password = "", quint32 props = DEFAULT_LOGIN_PROPS);
    int removeUser(const QString &name);
    int showAllUsers();

    quint32 getNumberOfUsers() { return this->m_lUserLogin.size(); }

    bool userExists(QString name);
    bool userExists(quint32 index);
    bool userCheckPassword(QString name, QString passw);
    bool userChangePassword(QString name, QString passw);
    bool userChangeProperties(QString name, quint32 props);
    bool userChangeReadName(QString name, QString readName);
    quint32 getUserProperties(QString name);
    QString getReadableName(QString name);

private:
    QSettings           *m_pUserSettings = NULL;
    QList<UserLogin>    m_lUserLogin;
    QMutex              m_mUserIniMutex;
    QMutex              m_mUserListMutex;

    QList<UserLogin>    m_lAddUserLoginProblems;

    void saveActualUserList();

    bool addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, QString readname, bool checkUser = true);
    void addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, QString readname, QList<UserLogin> *pList);
    quint32 getUserLoginIndex(const QString &name);
    quint32 getNextLoginIndex();

    bool updateUserLoginValue(UserLogin *pUserLog, QString key, QVariant value);

};

#endif // LISTEDUSER_H
