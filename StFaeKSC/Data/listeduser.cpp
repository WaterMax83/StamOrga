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

ListedUser* g_ListedUser = NULL;

ListedUser::ListedUser()
{
    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/ListedUsers.ini";

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for UserSettings"));
        return;
    }
    g_ListedUser = this;

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    this->m_hash = new QCryptographicHash(QCryptographicHash::Sha3_512);
#else
    this->m_hash = new QCryptographicHash(QCryptographicHash::Keccak_512);
#endif

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            QString name      = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            quint32 index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toUInt();
            qint64  timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();

            QString passw    = this->m_pConfigSettings->value(LOGIN_PASSWORD, "").toString();
            QString salt     = this->m_pConfigSettings->value(LOGIN_SALT, "").toString();
            QString readname = this->m_pConfigSettings->value(LOGIN_READNAME, "").toString();
            quint32 prop     = this->m_pConfigSettings->value(LOGIN_PROPERTIES, 0x0).toUInt();

            if (salt == "") {
                salt      = createRandomString(8);
                passw     = this->createHashPassword(passw, salt);
                bProblems = true;
            }

            UserLogin* login = new UserLogin(name, timestamp, index, passw, salt, prop, readname);
            if (!this->addNewUserLogin(login))
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
        this->addNewUserLogin(pLogin);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();
}

int ListedUser::addNewUser(const QString name, const QString password, quint32 props)
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

    qint64  timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString salt      = createRandomString(8);

    QString intPassWord = password;
    if (password == "")
        intPassWord      = name;
    QString hashPassword = this->createHashPassword(intPassWord, salt);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, name);
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(LOGIN_PASSWORD, hashPassword);
    this->m_pConfigSettings->setValue(LOGIN_SALT, salt);
    this->m_pConfigSettings->setValue(LOGIN_PROPERTIES, props);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    UserLogin* login = new UserLogin(name, timestamp, newIndex, hashPassword, salt, 0x0, "");
    this->addNewUserLogin(login, false);

    qInfo().noquote() << QString("Added new user: %1").arg(name);
    return newIndex;
}

QString ListedUser::showAllUsers()
{
    QString      rValue;
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    qint32 mxSzName = 0, mxSzRdName = 0;
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;

        if (pLogin->m_itemName.size() > mxSzName)
            mxSzName = pLogin->m_itemName.size();
        if (pLogin->m_readName.size() > mxSzRdName)
            mxSzRdName = pLogin->m_readName.size();
    }

    mxSzName += 2;
    mxSzRdName += 2;
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        QString output = QString("%1 - %2").arg(pLogin->m_itemName, -mxSzName).arg(pLogin->m_readName, -mxSzRdName);
        output.append(QString(" : 0x%1\n").arg(QString::number(pLogin->m_properties, 16)));
        rValue.append(output);
    }

    return rValue;
}

void ListedUser::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        UserLogin* pItem = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(LOGIN_PASSWORD, pItem->m_password);
        this->m_pConfigSettings->setValue(LOGIN_SALT, pItem->m_salt);
        this->m_pConfigSettings->setValue(LOGIN_READNAME, pItem->m_readName);
        this->m_pConfigSettings->setValue(LOGIN_PROPERTIES, pItem->m_properties);
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

            QString hashPassWord = this->createHashPassword(passw, pLogin->m_salt);
            if (pLogin->m_password == hashPassWord)
                return true;
            return false;
        }
    }
    return false;
}

qint32 ListedUser::userCheckPasswordHash(QString name, QString hash, QString random)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (name.length() < MIN_SIZE_USERNAME)
        return ERROR_CODE_WRONG_SIZE;

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        if (pLogin->m_itemName == name) {

            QString passWordWithRandowm = this->createHashPassword(pLogin->m_password, random);
            if (passWordWithRandowm == hash)
                return ERROR_CODE_SUCCESS;
            return ERROR_CODE_WRONG_PASSWORD;
        }
    }
    return ERROR_CODE_NOT_FOUND;
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

            QString hashPassWord = this->createHashPassword(passw, pLogin->m_salt);
            if (this->updateItemValue(pLogin, LOGIN_PASSWORD, QVariant(hashPassWord))) {
                pLogin->m_password = hashPassWord;
                return true;
            }
        }
    }
    return false;
}

bool ListedUser::userChangePasswordHash(QString name, QString passw)
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
                pLogin->m_password = passw;
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
                pLogin->m_properties = props;
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
                pLogin->m_readName = readName;
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
            return pLogin->m_properties;
    }
    return 0;
}

QString ListedUser::getReadableName(quint32 userIndex)
{
    UserLogin* pLogin = (UserLogin*)(this->getItem(userIndex));
    if (pLogin == NULL)
        return "";

    QMutexLocker locker(&this->m_mInternalInfoMutex);

    return pLogin->m_readName;
}

QString ListedUser::getSalt(QString name)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    for (int i = 0; i < this->getNumberOfInternalList(); i++) {
        UserLogin* pLogin = (UserLogin*)(this->getItemFromArrayIndex(i));
        if (pLogin == NULL)
            continue;
        if (pLogin->m_itemName == name)
            return pLogin->m_salt;
    }
    return "";
}

bool ListedUser::addNewUserLogin(UserLogin* login, bool checkItem)
{
    if (checkItem) {
        if (itemExists(login->m_itemName)) {
            qWarning().noquote() << QString("User \"%1\" already exists, not adding to internal list").arg(login->m_itemName);
            return false;
        }

        if (login->m_index == 0 || itemExists(login->m_index)) {
            qWarning().noquote() << QString("User \"%1\" with index \"%2\" already exists, saving with new index").arg(login->m_itemName).arg(login->m_index);
            this->addNewConfigItem(login, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(login, &this->m_lInteralList);
    return true;
}

QString ListedUser::createHashPassword(const QString passWord, const QString salt)
{
    this->m_hash->reset();
    QByteArray tmp = passWord.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());
    tmp = salt.toUtf8();
    this->m_hash->addData(tmp.constData(), tmp.length());

    QString hashPassword(this->m_hash->result());

    return hashPassword;
}

ListedUser::~ListedUser()
{
    delete this->m_hash;

    if (this->m_pConfigSettings != NULL)
        delete this->m_pConfigSettings;
}
