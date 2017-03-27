#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

#include <iostream>

#include "listeduser.h"
#include "../Common/General/globalfunctions.h"

ListedUser::ListedUser()
{
    QString userSetFilePath = QCoreApplication::applicationDirPath() + "/Settings/ListedUsers.ini";

    if (!CheckFilePathExistAndCreate(userSetFilePath))
    {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return;
    }

    this->m_pUserSettings = new QSettings(userSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mUserIniMutex);

        this->m_pUserSettings->beginGroup(USER_GROUP);
        int sizeOfLogins = this->m_pUserSettings->beginReadArray(LOGIN_ARRAY);

        for (int i=0; i<sizeOfLogins; i++ ) {
            this->m_pUserSettings->setArrayIndex(i);
            QString name = this->m_pUserSettings->value(LOGIN_USERNAME, "").toString();
            QString passw = this->m_pUserSettings->value(LOGIN_PASSWORD, "").toString();
            quint32 prop = this->m_pUserSettings->value(LOGIN_PROPERTIES, 0x0).toInt();
            quint32 index = this->m_pUserSettings->value(LOGIN_INDEX, 0).toInt();
            if (!this->addNewUserLogin(name, passw, prop, index))
                bProblems = true;
        }
        this->m_pUserSettings->endArray();
        this->m_pUserSettings->endGroup();
    }


    for (int i=0; i<this->m_lAddUserLoginProblems.size(); i++)
    {
        bProblems = true;
        this->m_lAddUserLoginProblems[i].index = this->getNextLoginIndex();
        this->addNewUserLogin(this->m_lAddUserLoginProblems[i].userName, this->m_lAddUserLoginProblems[i].password,
                              this->m_lAddUserLoginProblems[i].properties, this->m_lAddUserLoginProblems[i].index);
    }

    if (bProblems)
        this->saveActualUserList();
}

int ListedUser::addNewUser(const QString &name, const QString &password, quint32 props)
{
    if (name.length() < MIN_SIZE_USERNAME) {
        CONSOLE_WARNING (QString("Name \"%1\" is too short").arg(name));
        return -1;
    }

    if (this->userExists(name)) {
        CONSOLE_WARNING (QString("User \"%1\" already exists").arg(name));
        return -1;
    }

    QMutexLocker locker(&this->m_mUserIniMutex);

    QString lPassword = password;
    if (password == "")
        lPassword = name;

    int newIndex = this->getNextLoginIndex();
    this->m_pUserSettings->beginGroup(USER_GROUP);
    this->m_pUserSettings->beginWriteArray(LOGIN_ARRAY);
    this->m_pUserSettings->setArrayIndex(this->getNumberOfUsers());
    this->m_pUserSettings->setValue(LOGIN_USERNAME, name);
    this->m_pUserSettings->setValue(LOGIN_PASSWORD, lPassword);
    this->m_pUserSettings->setValue(LOGIN_PROPERTIES, props);
    this->m_pUserSettings->setValue(LOGIN_INDEX, newIndex);
    this->m_pUserSettings->endArray();
    this->m_pUserSettings->endGroup();
    this->m_pUserSettings->sync();

    this->addNewUserLogin(name, name, 0x0, newIndex, false);

    CONSOLE_INFO(QString("Added new user: %1").arg(name));
    return newIndex;
}

int ListedUser::removeUser(const QString &name)
{
    int index = this->getUserLoginIndex(name);
    if (index < 0 || index > this->m_lUserLogin.size() - 1)
    {
        CONSOLE_WARNING(QString("Could not find user \"%1\"").arg(name));
        return -1;
    }

    QMutexLocker locker(&this->m_mUserListMutex);

    this->m_lUserLogin.removeAt(index);

    this->saveActualUserList();

    CONSOLE_INFO(QString("removed User \"%1\"").arg(name));
    return 0;
}

int ListedUser::showAllUsers()
{
    QMutexLocker locker(&this->m_mUserListMutex);

    foreach (UserLogin login, this->m_lUserLogin) {
        std::cout << login.userName.toStdString() << std::endl;
    }
    return 0;
}

void ListedUser::saveActualUserList()
{
    QMutexLocker locker(&this->m_mUserIniMutex);

    this->m_pUserSettings->beginGroup(USER_GROUP);
    this->m_pUserSettings->remove("");              // clear all elements

    this->m_pUserSettings->beginWriteArray(LOGIN_ARRAY);
    for (int i=0; i<this->m_lUserLogin.size(); i++) {
        this->m_pUserSettings->setArrayIndex(i);
        this->m_pUserSettings->setValue(LOGIN_USERNAME, this->m_lUserLogin[i].userName);
        this->m_pUserSettings->setValue(LOGIN_PASSWORD, this->m_lUserLogin[i].password);
        this->m_pUserSettings->setValue(LOGIN_PROPERTIES, this->m_lUserLogin[i].properties);
        this->m_pUserSettings->setValue(LOGIN_INDEX, this->m_lUserLogin[i].index);
    }

    this->m_pUserSettings->endArray();
    this->m_pUserSettings->endGroup();

    qDebug().noquote() << QString("saved actual User List with %1 entries").arg(this->m_lUserLogin.size());
}

bool ListedUser::userExists(QString name)
{
    QMutexLocker locker(&this->m_mUserListMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;
    foreach (UserLogin login, this->m_lUserLogin) {
        if (login.userName == name)
            return true;
    }
    return false;
}

bool ListedUser::userExists(quint32 index)
{
    QMutexLocker locker(&this->m_mUserListMutex);

    foreach (UserLogin login, this->m_lUserLogin) {
        if (login.index == index)
            return true;
    }
    return false;
}

bool ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, bool checkUser)
{
    if (checkUser) {
        if (userExists(name)) {
            qWarning().noquote() << QString("User \"%1\" already exists, not adding to internal list").arg(name);
            return false;
        }

        if (index == 0 || userExists(index)) {
            qWarning().noquote() << QString("User \"%1\" with index \"%2\" already exists, saving with new index").arg(name).arg(index);
            this->addNewUserLogin(name, password, prop, index, &this->m_lAddUserLoginProblems);
            return false;
        }
    }

    this->addNewUserLogin(name, password, prop, index, &this->m_lUserLogin);
    return true;
}

void ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index, QList<UserLogin> *pList)
{
    QMutexLocker locker(&this->m_mUserListMutex);

    UserLogin login;
    login.userName = name;
    login.password = password;
    login.properties = prop;
    login.index = index;
    pList->append(login);
}

quint32 ListedUser::getUserLoginIndex(const QString &name)
{
    QMutexLocker locker(&this->m_mUserListMutex);

    for (int i=0; i<this->m_lUserLogin.size(); i++) {
        if (this->m_lUserLogin[i].userName == name)
            return i;
    }
    return -1;
}

quint32 ListedUser::getNextLoginIndex()
{
    QMutexLocker locker(&this->m_mUserListMutex);

    quint32 index = 0;
    for (int i=0; i<this->m_lUserLogin.size(); i++) {
        if (this->m_lUserLogin[i].index > index)
            index = this->m_lUserLogin[i].index;
    }
    return index+1;
}


ListedUser::~ListedUser()
{
    if (this->m_pUserSettings != NULL)
        delete this->m_pUserSettings;
}
