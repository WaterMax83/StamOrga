#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>

#include <iostream>

#include "listeduser.h"
#include "../Common/General/globalfunctions.h"

ListedUser::ListedUser()
{
    QString userSetFilePath = getUserHomeConfigPath() + "/Settings/ListedUsers.ini";

    if (!checkFilePathExistAndCreate(userSetFilePath))
    {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return;
    }

    this->m_pConfigSettings = new QSettings(userSetFilePath, QSettings::IniFormat);

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfLogins = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i=0; i<sizeOfLogins; i++ ) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            quint32 index = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64 timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

            QString passw = this->m_pConfigSettings->value(LOGIN_PASSWORD, "").toString();
            QString readname = this->m_pConfigSettings->value(LOGIN_READNAME, "").toString();
            quint32 prop = this->m_pConfigSettings->value(LOGIN_PROPERTIES, 0x0).toUInt();

            if (!this->addNewUserLogin(name, timestamp, index, passw, prop, readname))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i=0; i<this->m_lAddItemProblems.size(); i++)
    {
        bProblems = true;
        this->m_lAddItemProblems[i].m_index = this->getNextInternalIndex();
        this->addNewUserLogin(this->m_lAddItemProblems[i].m_itemName, this->m_lAddItemProblems[i].m_timestamp,
                              this->m_lAddItemProblems[i].m_index, this->m_lAddItemProblems[i].password,
                              this->m_lAddItemProblems[i].properties, this->m_lAddItemProblems[i].readName);
    }

    if (bProblems)
        this->saveCurrentInteralList();
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

    int newIndex = this->getNextInternalIndex();

    QMutexLocker locker(&this->m_mConfigIniMutex);

    QString lPassword = password;
    if (password == "")
        lPassword = name;

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, name);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(LOGIN_PASSWORD, lPassword);
    this->m_pConfigSettings->setValue(LOGIN_PROPERTIES, props);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->addNewUserLogin(name, timestamp, newIndex, name, 0x0, "",false);

    CONSOLE_INFO(QString("Added new user: %1").arg(name));
    return newIndex;
}

int ListedUser::removeUser(const QString &name)
{
    int index = this->getUserLoginIndex(name);

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i=0; i < this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_index == index) {
            this->m_lInteralList.removeAt(i);
            this->saveCurrentInteralList();

            CONSOLE_INFO(QString("removed User \"%1\"").arg(name));
            return ERROR_CODE_SUCCESS;
        }
    }

    CONSOLE_WARNING(QString("Could not find user \"%1\"").arg(name))
    return ERROR_CODE_COMMON;
}

int ListedUser::showAllUsers()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (UserLogin login, this->m_lInteralList) {
        std::cout << login.m_itemName.toStdString()
                  << " - " << login.readName.toStdString()
                  << " : 0x" << QString::number(login.properties, 16).toStdString()
                  << std::endl;
    }
    return 0;
}

void ListedUser::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove("");              // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i=0; i<this->m_lInteralList.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, this->m_lInteralList[i].m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, this->m_lInteralList[i].m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, this->m_lInteralList[i].m_index);

        this->m_pConfigSettings->setValue(LOGIN_PASSWORD, this->m_lInteralList[i].password);
        this->m_pConfigSettings->setValue(LOGIN_READNAME, this->m_lInteralList[i].readName);
        this->m_pConfigSettings->setValue(LOGIN_PROPERTIES, this->m_lInteralList[i].properties);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved actual User List with %1 entries").arg(this->m_lInteralList.size());
}

bool ListedUser::userExists(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;
    foreach (UserLogin login, this->m_lInteralList) {
        if (login.m_itemName == name)
            return true;
    }
    return false;
}

bool ListedUser::userExists(quint32 index)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (UserLogin login, this->m_lInteralList) {
        if (login.m_index == index)
            return true;
    }
    return false;
}

bool ListedUser::userCheckPassword(QString name, QString passw)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;

    foreach (UserLogin login, this->m_lInteralList) {
        if (login.m_itemName == name) {
            if (login.password == passw)
                return true;
            return false;
        }
    }
    return false;
}

bool ListedUser::userChangePassword(QString name, QString passw)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;

    for (int i=0; i<this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == name) {
            if (this->updateUserLoginValue(&this->m_lInteralList[i], LOGIN_PASSWORD, QVariant(passw))) {
                this->m_lInteralList[i].password = passw;
                return true;
            }
        }
    }
    return false;
}

bool ListedUser::userChangeProperties(QString name, quint32 props)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;

    for (int i=0; i<this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == name) {
            if (this->updateUserLoginValue(&this->m_lInteralList[i], LOGIN_PROPERTIES, QVariant(props))) {
                this->m_lInteralList[i].properties = props;
                return true;
            } else
                return false;
        }
    }
    return false;
}

bool ListedUser::userChangeReadName(QString name, QString readName)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME || readName.length() < 3)
        return false;

    for (int i=0; i<this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == name) {
            if (this->updateUserLoginValue(&this->m_lInteralList[i], LOGIN_READNAME, QVariant(readName))) {
                this->m_lInteralList[i].readName = readName;
                return true;
            } else
                return false;
        }
    }
    return false;
}

quint32 ListedUser::getUserProperties(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (UserLogin login, this->m_lInteralList) {
        if (login.m_itemName == name)
            return login.properties;
    }
    return 0;
}

QString ListedUser::getReadableName(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    foreach (UserLogin login, this->m_lInteralList) {
        if (login.m_itemName == name)
            return login.readName;
    }
    return "";
}

bool ListedUser::addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, bool checkUser)
{
    if (checkUser) {
        if (userExists(name)) {
            qWarning().noquote() << QString("User \"%1\" already exists, not adding to internal list").arg(name);
            return false;
        }

        if (index == 0 || userExists(index)) {
            qWarning().noquote() << QString("User \"%1\" with index \"%2\" already exists, saving with new index").arg(name).arg(index);
            this->addNewUserLogin(name, timestamp, index, password, prop, readname, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewUserLogin(name, timestamp, index, password, prop, readname, &this->m_lInteralList);
    return true;
}

void ListedUser::addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, QList<UserLogin> *pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    UserLogin login;
    login.m_itemName = name;
    login.m_timestamp = timestamp;
    login.m_index = index;
    login.password = password;
    login.readName = readname;
    login.properties = prop;

    pList->append(login);
}

qint32 ListedUser::getUserLoginIndex(const QString &name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i=0; i<this->m_lInteralList.size(); i++) {
        if (this->m_lInteralList[i].m_itemName == name)
            return this->m_lInteralList[i].m_index;
    }
    return -1;
}

bool ListedUser::updateUserLoginValue(UserLogin *pUserLog, QString key, QVariant value)
{
    bool rValue = false;
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    int arrayCount = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);
    for (int i=0; i<arrayCount; i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        QString actName = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
        quint32 actIndex = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
        if (pUserLog->m_itemName == actName && pUserLog->m_index == actIndex) {

            this->m_pConfigSettings->setValue(key, value);
            qInfo().noquote() << QString("Change %1 of user %2 to %3").arg(key).arg(pUserLog->m_itemName).arg(value.toString());
            rValue = true;
            break;
        }
    }
    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    return rValue;
}


ListedUser::~ListedUser()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
