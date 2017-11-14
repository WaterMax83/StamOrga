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

#include "../Common/General/globalfunctions.h"
#include "userevents.h"


UserEvents::UserEvents()
{
}

qint32 UserEvents::initialize(QString type, QString info, qint32 userID)
{
    this->m_type      = type;
    this->m_info      = info;
    this->m_timestamp = QDateTime::currentMSecsSinceEpoch();

    QString configSetFilePath = getUserHomeConfigPath() + "/Settings/UserEvents/";
    configSetFilePath.append(QString("Event_%1.ini").arg(this->m_timestamp));

    if (!checkFilePathExistAndCreate(configSetFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for User Events"));
        return ERROR_CODE_COMMON;
    }

    this->m_pConfigSettings = new QSettings(configSetFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("UserEventHeader");

    this->m_pConfigSettings->setValue("type", this->m_type);
    this->m_pConfigSettings->setValue("info", this->m_info);
    this->m_pConfigSettings->setValue("timestamp", this->m_timestamp);

    this->m_pConfigSettings->endGroup();

    if (userID > 0)
        this->addNewUser(userID);

    return ERROR_CODE_SUCCESS;
}

qint32 UserEvents::initialize(QString filePath)
{
    this->m_pConfigSettings = new QSettings(filePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    this->m_pConfigSettings->beginGroup("UserEventHeader");

    this->m_type      = this->m_pConfigSettings->value("type", 0).toString();
    this->m_info      = this->m_pConfigSettings->value("info", 0).toString();
    this->m_timestamp = this->m_pConfigSettings->value("timestamp", 0).toLongLong();

    this->m_pConfigSettings->endGroup();

    if (this->m_type == "" || this->m_timestamp == 0)
        return ERROR_CODE_COMMON;

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);

            quint32 userID = this->m_pConfigSettings->value(EVENT_USER_ID, 0x0).toUInt();

            if (userID != 0 && !this->m_lUserIDs.contains(userID))
                this->m_lUserIDs.append(userID);
            else
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }

    if (bProblems)
        this->saveCurrentInteralList();

    return ERROR_CODE_SUCCESS;
}


bool UserEvents::addNewUser(const quint32 userID)
{
    QMutexLocker locker(&this->m_mConfigIniMutex);
    QMutexLocker locker2(&this->m_mInternalInfoMutex);

    if (this->m_lUserIDs.contains(userID))
        return false;

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->m_lUserIDs.size());

    this->m_pConfigSettings->setValue(EVENT_USER_ID, userID);

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    this->m_lUserIDs.append(userID);

    return true;
}

bool UserEvents::getHasUserGotEvent(const quint32 userID)
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    if (this->m_lUserIDs.contains(userID))
        return true;
    return false;
}


void UserEvents::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->m_lUserIDs.size(); i++) {
        this->m_pConfigSettings->setArrayIndex(i);
        this->m_pConfigSettings->setValue(EVENT_USER_ID, this->m_lUserIDs.at(i));
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    // qDebug().noquote() << QString("saved current User List with %1 entries").arg(this->getNumberOfInternalList());
}

QString UserEvents::getInfo()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    return this->m_info;
}

QString UserEvents::getType()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    return this->m_type;
}


UserEvents::~UserEvents()
{
}
