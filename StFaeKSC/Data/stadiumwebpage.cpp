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

#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include "../Common/General/globalfunctions.h"
#include "stadiumwebpage.h"

// clang-format off
#define WEBPAGE_USER_ID         "userID"
#define WEBPAGE_LASTUPDATE      "lastUpdate"
#define WEBPAGE_LINK            "link"

// clang-format on

StadiumWebPage::StadiumWebPage()
{
}

qint32 StadiumWebPage::initialize()
{
    QString stadiumWebPageFilePath = getUserHomeConfigPath() + "/Settings/StadiumWebPage/StadiumWebPage.ini";

    if (!checkFilePathExistAndCreate(stadiumWebPageFilePath)) {
        CONSOLE_CRITICAL(QString("Could not create File for stadium webpage setting"));
        return ERROR_CODE_NOT_POSSIBLE;
    }

    this->m_webPageFolder = getUserHomeConfigPath() + "/Settings/StadiumWebPage";
    QDir dir(this->m_webPageFolder);
    if (!dir.exists())
        dir.mkdir(this->m_webPageFolder);

    this->m_pConfigSettings = new QSettings(stadiumWebPageFilePath, QSettings::IniFormat);
    this->m_pConfigSettings->setIniCodec(("UTF-8"));

    /* Check wheter we have to save data after reading again */
    bool bProblems = false;
    {
        QMutexLocker locker(&this->m_mConfigIniMutex);

        this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
        int sizeOfArray = this->m_pConfigSettings->beginReadArray(CONFIG_LIST_ARRAY);

        for (int i = 0; i < sizeOfArray; i++) {
            this->m_pConfigSettings->setArrayIndex(i);
            WebPageItem* pWebPage = new WebPageItem();
            pWebPage->m_itemName  = this->m_pConfigSettings->value(ITEM_NAME, "").toString();
            pWebPage->m_index     = this->m_pConfigSettings->value(ITEM_INDEX, 0).toInt();
            pWebPage->m_timestamp = this->m_pConfigSettings->value(ITEM_TIMESTAMP, 0x0).toLongLong();

            pWebPage->m_userID     = this->m_pConfigSettings->value(WEBPAGE_USER_ID, 0).toInt();
            pWebPage->m_lastUpdate = this->m_pConfigSettings->value(WEBPAGE_LASTUPDATE, 0x0).toLongLong();
            pWebPage->m_link       = this->m_pConfigSettings->value(WEBPAGE_LINK, 0x0).toString();

            if (!this->addNewStadiumWebPage(pWebPage))
                bProblems = true;
        }
        this->m_pConfigSettings->endArray();
        this->m_pConfigSettings->endGroup();
    }
    if (this->readLastUpdateTime() == 0)
        this->setNewUpdateTime();

    for (int i = 0; i < this->m_lAddItemProblems.size(); i++) {
        bProblems             = true;
        WebPageItem* pWebPage = (WebPageItem*)(this->getProblemItemFromArrayIndex(i));
        if (pWebPage == NULL)
            continue;
        pWebPage->m_index = this->getNextInternalIndex();
        this->addNewStadiumWebPage(pWebPage);
    }
    this->m_lAddItemProblems.clear();

    if (bProblems)
        this->saveCurrentInteralList();

    this->sortWebPages();

    return ERROR_CODE_SUCCESS;
}

qint32 StadiumWebPage::addNewWebPageItem(const qint32 userID)
{

    int     newIndex = this->getNextInternalIndex();
    QMutexLocker locker(&this->m_mConfigIniMutex);

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    this->m_pConfigSettings->setArrayIndex(this->getNumberOfInternalList());

    this->m_pConfigSettings->setValue(ITEM_NAME, "");
    this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, timestamp);
    this->m_pConfigSettings->setValue(ITEM_INDEX, newIndex);

    this->m_pConfigSettings->setValue(WEBPAGE_USER_ID, userID);
    this->m_pConfigSettings->setValue(WEBPAGE_LASTUPDATE, timestamp);
    this->m_pConfigSettings->setValue(WEBPAGE_LINK, "");

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();
    this->m_pConfigSettings->sync();

    WebPageItem* info = new WebPageItem();
    info->m_itemName  = "";
    info->m_timestamp = timestamp;
    info->m_index     = newIndex;
    info->m_userID    = userID;
    info->m_lastUpdate = timestamp;
    info->m_link = "";
    this->addNewStadiumWebPage(info, false);

    this->sortWebPages();


    return ERROR_CODE_SUCCESS;
}

qint32 StadiumWebPage::loadWebPageDataItem(const qint32 index, QString &text, QString &body)
{
    WebPageItem* pItem = (WebPageItem*) this->getItem(index);
    if (pItem == NULL)
        return ERROR_CODE_NOT_FOUND;

    text = pItem->m_itemName;
    body = "";
    if (!pItem->m_link.isEmpty()) {
        QFile linkFile(pItem->m_link);
        if (linkFile.open(QIODevice::ReadOnly)) {

            body = qCompress(linkFile.readAll(), 9).toHex();
            linkFile.close();
        }
    }

    return ERROR_CODE_SUCCESS;
}

qint32 StadiumWebPage::setWebPageDataItem(const qint32 index, const QString text, QString body)
{
    WebPageItem* pItem = (WebPageItem*) this->getItem(index);
    if (pItem == NULL)
        return ERROR_CODE_NOT_FOUND;

    QByteArray uBody;
    if (!body.isEmpty())
        uBody = qUncompress(QByteArray::fromHex(body.toUtf8()));
    qint64 lastUpdate = QDateTime::currentMSecsSinceEpoch();

    if (pItem->m_itemName != text) {
        pItem->m_itemName = text;
        this->updateItemValue(pItem, ITEM_NAME, QVariant(text));
    }

    if (pItem->m_link.isEmpty()) {
        pItem->m_link = QString("%1/WebPage_%2.html").arg(this->m_webPageFolder).arg(pItem->m_index);
        this->updateItemValue(pItem, WEBPAGE_LINK, QVariant(pItem->m_link));
    }

    QFile linkFile(pItem->m_link);
    if (!linkFile.open(QIODevice::WriteOnly))
        return ERROR_CODE_NOT_POSSIBLE;

    linkFile.write(uBody);
    linkFile.flush();
    linkFile.close();

    pItem->m_lastUpdate = lastUpdate;
    this->updateItemValue(pItem, WEBPAGE_LASTUPDATE, QVariant(lastUpdate));

    return ERROR_CODE_SUCCESS;
}

void StadiumWebPage::saveCurrentInteralList()
{
    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(GROUP_LIST_ITEM);
    this->m_pConfigSettings->remove(""); // clear all elements

    this->m_pConfigSettings->beginWriteArray(CONFIG_LIST_ARRAY);
    for (int i = 0; i < this->getNumberOfInternalList(); i++) {

        WebPageItem* pItem = (WebPageItem*)(this->getItemFromArrayIndex(i));
        if (pItem == NULL)
            continue;
        this->m_pConfigSettings->setArrayIndex(i);

        this->m_pConfigSettings->setValue(ITEM_NAME, pItem->m_itemName);
        this->m_pConfigSettings->setValue(ITEM_TIMESTAMP, pItem->m_timestamp);
        this->m_pConfigSettings->setValue(ITEM_INDEX, pItem->m_index);

        this->m_pConfigSettings->setValue(WEBPAGE_USER_ID, pItem->m_userID);
        this->m_pConfigSettings->setValue(WEBPAGE_LASTUPDATE, pItem->m_lastUpdate);
        this->m_pConfigSettings->setValue(WEBPAGE_LINK, pItem->m_link);
    }

    this->m_pConfigSettings->endArray();
    this->m_pConfigSettings->endGroup();

    this->setNewUpdateTime();

    qDebug().noquote() << QString("saved webpage infos %1 with %2 entries").arg(this->m_pConfigSettings->fileName()).arg(this->getNumberOfInternalList());
}

void StadiumWebPage::sortWebPages()
{
    QMutexLocker locker(&this->m_mInternalInfoMutex);

    std::sort(this->m_lInteralList.begin(), this->m_lInteralList.end(), ConfigItem::compareTimeStampFunctionAscending);
}

bool StadiumWebPage::addNewStadiumWebPage(WebPageItem* pWebPage, bool checkItem)
{
    if (checkItem) {
        if (pWebPage->m_index == 0 || itemExists(pWebPage->m_index)) {
            qWarning().noquote() << QString("Media info with index \"%1\" already exists, saving with new index").arg(pWebPage->m_index);
            this->addNewConfigItem(pWebPage, &this->m_lAddItemProblems);
            return false;
        }
    }

    this->addNewConfigItem(pWebPage, &this->m_lInteralList);
    return true;
}
