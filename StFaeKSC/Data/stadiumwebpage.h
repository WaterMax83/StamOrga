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


#ifndef STADIUMWEBPAGE_H
#define STADIUMWEBPAGE_H

#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"


class WebPageItem : public ConfigItem
{
public:
    WebPageItem() {}
    WebPageItem(QString name, qint32 index,
                qint64 timestamp, qint32 userID,
                qint64 lastUpdate, QString link)
    {
        this->m_itemName  = name;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_userID     = userID;
        this->m_lastUpdate = lastUpdate;
        this->m_link       = link;
    }

    qint32  m_userID;
    qint64  m_lastUpdate;
    QString m_link;
};

class StadiumWebPage : public ConfigList
{

public:
    explicit StadiumWebPage();

    qint32 initialize();

    qint32 addNewWebPageItem(const qint32 userID);
    qint32 loadWebPageDataItem(const qint32 index,  QString& text, QString& link);
    qint32 setWebPageDataItem(const qint32 index, const QString text, QString link);

    virtual qint32 checkConsistency() { return -12; }

protected:
private:
//    QString m_webPageFolder;

    void saveCurrentInteralList() override;

    void sortWebPages();

    bool addNewStadiumWebPage(WebPageItem* media, bool checkItem = true);
};

#endif // STADIUMWEBPAGE_H
