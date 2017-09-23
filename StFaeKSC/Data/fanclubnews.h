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

#ifndef FANCLUBNEWS_H
#define FANCLUBNEWS_H

#include "../Common/General/globalfunctions.h"
#include "configlist.h"


// clang-format off
#define NEWS_DATA_TEXT      "NewsText"
#define NEWS_DATA_USERID    "UserID"
// clang-format on

class NewsData : public ConfigItem
{
public:
    NewsData(QString name, quint32 index,
             qint64 timestamp, QByteArray newsText,
             quint32 userID)
    {
        this->m_itemName  = name;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_newsText = newsText;
        this->m_userID   = userID;
    }

    QByteArray m_newsText;
    quint32 m_userID;
};

class FanclubNews : public ConfigList
{
public:
    FanclubNews();

    int addNewFanclubNews(const QString header, const QByteArray info, const quint32 userID);

    int changeFanclubNews(const quint32 newsIndex, const QString header, const QByteArray info, const quint32 userID);

    int showNewsData();

private:
    void saveCurrentInteralList() override;

    bool addNewNewsData(NewsData* news, bool checkItem = true);
};

#endif // FANCLUBNEWS_H
