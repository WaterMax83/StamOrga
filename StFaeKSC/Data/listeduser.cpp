/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>

#include <iostream>

#include "../Common/General/globalfunctions.h"
#include "listeduser.h"

ListedUser::ListedUser()
{
    QString userSetFilePath = getUserHomeConfigPath() + "/Settings/ListedUsers.ini";

    if (!checkFilePathExistAndCreate(userSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return;
    }

    this->m_pConfigSettings = new QSettings(userSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfLogins = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfLogins; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toULongLong();

            QString passw    = this->m_pConfigSettings->value(LOGIN_PASSWORD, "").toString();
            QString readname = this->m_pConfigSettings->value(LOGIN_READNAME, "").toString();
            quint32 prop     = this->m_pConfigSettings->value(LOGIN_PROPERTIES, 0x0).toUInt();

            if (!this->addNewUserLogin(name, timestamp, index, passw, prop, readname))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }


    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems         = true;
        UserLogin* pLogin = (UserLogin*)(this->getProblemItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        pLogin->m_index = this->getNextInternalIndex();
        this->addNewUserLogin(pLogin->m_itemName, pLogin->m_timestamp,
                              pLogin->m_index, pLogin->password,
                              pLogin->properties, pLogin->readName);

        delete pLogin;
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

int ListedUser::addNewUser(const QString& name, const QString& password, quint32 props)
{
    if (name.length() < MIN_SIZE_USERNAME) {
        CONSOLE_WARNING(QString("Name \"%1\" is too short").arg(name));
        return -1;
    }

    if (this->itemExists(name)) {
        CONSOLE_WARNING(QString("User \"%1\" already exists").arg(name));
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

    this->addNewUserLogin(name, timestamp, newIndex, name, 0x0, "", false);

    qInfo(QString("Added new user: %1").arg(name));
    return newIndex;
}

int ListedUser::showAllUsers()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        std::cout << pLogin->m_itemName.toStdString()
                  << " - " << pLogin->readName.toStdString()
                  << " : 0x" << QString::number(pLogin->properties, 16).toStdString()
                  << std::endl;
    }

    return 0;
}

void ListedUser::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pLogin->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pLogin->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pLogin->m_index);

        this->m_pConfigSettings->setValue(LOGIN_PASSWORD, pLogin->password);
        this->m_pConfigSettings->setValue(LOGIN_READNAME, pLogin->readName);
        this->m_pConfigSettings->setValue(LOGIN_PROPERTIES, pLogin->properties);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    qDebug().noquote() << QString("saved current User List with %1 entries").arg(this->getNumberOfInternalList());
}

bool ListedUser::userCheckPassword(QString name, QString passw)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return false;

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        if (pLogin->m_itemName == name) {
            if (pLogin->password == passw)
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

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;

        if (pLogin->m_itemName == name) {
            if (this->updateItemValue(pLogin, LOGIN_PASSWORD, QVariant(passw))) {
                pLogin->password = passw;
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

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;

        if (pLogin->m_itemName == name) {
            if (this->updateItemValue(pLogin, LOGIN_PROPERTIES, QVariant(props))) {
                pLogin->properties = props;
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

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;

        if (pLogin->m_itemName == name) {
            if (this->updateItemValue(pLogin, LOGIN_READNAME, QVariant(readName))) {
                pLogin->readName = readName;
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

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        if (pLogin->m_itemName == name)
            return pLogin->properties;
    }
    return 0;
}

QString ListedUser::getReadableName(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        if (pLogin->m_itemName == name)
            return pLogin->readName;
    }
    return "";
}

bool ListedUser::addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, bool checkUser)
{
    if (checkUser) {
        if (itemExists(name)) {
            qWarning().noquote() << QString("User \"%1\" already exists, not adding to internal list").arg(name);
            return false;
        }

        if (index == 0 || itemExists(index)) {
            qWarning().noquote() << QString("User \"%1\" with index \"%2\" already exists, saving with new index").arg(name).arg(index);
            this->addNewUserLogin(name, timestamp, index, password, prop, readname, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewUserLogin(name, timestamp, index, password, prop, readname, &this->m_lInteralList);
    return true;
}

void ListedUser::addNewUserLogin(QString name, qint64 timestamp, quint32 index, QString password, quint32 prop, QString readname, QList<ConfigItem*>* pList)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    UserLogin* login   = new UserLogin();
    login->m_itemName  = name;
    login->m_timestamp = timestamp;
    login->m_index     = index;
    login->password    = password;
    login->readName    = readname;
    login->properties  = prop;

    pList->append(login);
}


ListedUser::~ListedUser()
{
    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
