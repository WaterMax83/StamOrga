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


#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QtCore/QMutex>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include "configlist.h"


class MediaItem : public ConfigItem
{
public:
    MediaItem() {}
    MediaItem(QString name, qint32 index,
              qint64 timestamp, qint32 userID)
    {
        this->m_itemName  = name;
        this->m_index     = index;
        this->m_timestamp = timestamp;

        this->m_userID = userID;
    }

    qint32 m_userID;
};

class MediaInfo : public ConfigList
{

public:
    explicit MediaInfo();

    qint32 initialize(quint32 year, quint32 competition, quint32 seasonIndex, quint32 index);
    qint32 initialize(QString filePath);

    qint32 addNewMediaItem(const QString format, QByteArray& data, const qint32 userID);
    qint32 removeMediaItems(QStringList& lMedias);

    qint32 getGameIndex() { return this->m_gameIndex; }

    virtual qint32 checkConsistency() { return -12; }

protected:
private:
    quint32 m_year;
    quint32 m_competition;
    qint32  m_seasonIndex;
    qint32  m_gameIndex;

    void saveCurrentInteralList() override;

    void sortMedias();

    bool addNewMediaInfo(MediaItem* media, bool checkItem = true);
};

#endif // MEDIAINFO_H
