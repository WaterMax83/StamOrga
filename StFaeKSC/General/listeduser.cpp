#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

#include <iostream>

#include "listeduser.h"
#include "../Common/General/globalfunctions.h"


#define USER_GROUP              "USERS"
#define LOGIN_ARRAY             "logins"
#define LOGIN_USERNAME          "username"
#define LOGIN_PASSWORD          "password"
#define LOGIN_PROPERTIES        "properties"

#define MIN_SIZE_USERNAME       5

ListedUser::ListedUser()
{
    QString userSetFilePath = QCoreApplication::applicationDirPath() + "/Settings/ListedUsers.ini";

    if (!CheckFilePathExistAndCreate(userSetFilePath))
    {
        qDebug() << "Could not create File for UserSettings";
    }

    this->m_pUserSettings = new QSettings(userSetFilePath, QSettings::IniFormat);

    this->m_pUserSettings->beginGroup(USER_GROUP);
    int sizeOfLogins = this->m_pUserSettings->beginReadArray(LOGIN_ARRAY);
    for (int i=0; i<sizeOfLogins; i++ ) {
        this->m_pUserSettings->setArrayIndex(i);
        this->addNewUserLogin(this->m_pUserSettings->value(LOGIN_USERNAME).toString(),
                              this->m_pUserSettings->value(LOGIN_PASSWORD).toString(),
                              this->m_pUserSettings->value(LOGIN_PROPERTIES).toInt(),
                              i);
    }
    this->m_pUserSettings->endArray();
    this->m_pUserSettings->endGroup();
}

int ListedUser::addNewUser(const QString &name)
{
    if (name.length() < MIN_SIZE_USERNAME) {
        qWarning().noquote() << QString("Name \"%1\" is too short").arg(name);
        return -1;
    }

    if (this->userExists(name)) {
        qWarning().noquote() << QString("User \"%1\" already exists").arg(name);
        return -1;
    }

    int size = this->getNumberOfUsers();
    if (this->userExists(size)) {
        qWarning().noquote() << QString("User Index \"%1\" already exists").arg(size);
        return -1;
    }

    this->m_pUserSettings->beginGroup(USER_GROUP);
    this->m_pUserSettings->beginWriteArray(LOGIN_ARRAY);
    this->m_pUserSettings->setArrayIndex(size);
    this->m_pUserSettings->setValue(LOGIN_USERNAME, name);
    this->m_pUserSettings->setValue(LOGIN_PASSWORD, name);
    this->m_pUserSettings->setValue(LOGIN_PROPERTIES, 0x0);
    this->m_pUserSettings->endArray();
    this->m_pUserSettings->endGroup();
    this->m_pUserSettings->sync();

    this->addNewUserLogin(name, name, 0x0, size);

    qInfo().noquote() << QString("Added new user: %1").arg(name);
    return size;
}

int ListedUser::removeUser(const QString &name)
{
    int index = this->getUserLoginIndex(name);
    if (index < 0 || index > this->m_lUserLogin.size() - 1)
    {
        qWarning().noquote() << QString("Could not find user \"%1\"").arg(name);
        return -1;
    }
    this->m_lUserLogin.removeAt(index);

    this->m_pUserSettings->beginGroup(USER_GROUP);
    this->m_pUserSettings->remove("");              // clear all elements

    this->m_pUserSettings->beginWriteArray(LOGIN_ARRAY);
    for (int i=0; i<this->m_lUserLogin.size(); i++) {
        this->m_pUserSettings->setArrayIndex(i);
        this->m_pUserSettings->setValue(LOGIN_USERNAME, this->m_lUserLogin[i].userName);
        this->m_pUserSettings->setValue(LOGIN_PASSWORD, this->m_lUserLogin[i].password);
        this->m_pUserSettings->setValue(LOGIN_PROPERTIES, this->m_lUserLogin[i].properties);
    }

    this->m_pUserSettings->endArray();
    this->m_pUserSettings->endGroup();

    qInfo().noquote() << QString("remove User \"%1\"").arg(name);
    return 0;
}

int ListedUser::showAllUsers()
{
    foreach (UserLogin login, this->m_lUserLogin) {
        std::cout << login.userName.toStdString() << std::endl;
    }
    return 0;
}

bool ListedUser::userExists(QString name)
{
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
    foreach (UserLogin login, this->m_lUserLogin) {
        if (login.index == index)
            return true;
    }
    return false;
}

void ListedUser::addNewUserLogin(QString name, QString password, quint32 prop, quint32 index)
{
    UserLogin login;
    login.userName = name;
    login.password = password;
    login.properties = prop;
    login.index = index;
    this->m_lUserLogin.append(login);
}

int ListedUser::getUserLoginIndex(const QString &name)
{
    for (int i=0; i<this->m_lUserLogin.size(); i++)
    {
        if (this->m_lUserLogin[i].userName == name)
            return i;
    }
    return -1;
}


ListedUser::~ListedUser()
{
    if (this->m_pUserSettings != NULL)
        delete this->m_pUserSettings;
}
