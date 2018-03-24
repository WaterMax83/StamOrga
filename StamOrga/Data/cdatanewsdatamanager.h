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

#ifndef CDATANEWSDATAMANAGER_H
#define CDATANEWSDATAMANAGER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "newsdataitem.h"

class cDataNewsDataManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataNewsDataManager(QObject* parent = nullptr);

    qint32 initialize();

    void addNewNewsData(NewsDataItem* sNews, const quint16 updateIndex = 0);

    NewsDataItem* getNewsDataItem(qint32 newsIndex);

    Q_INVOKABLE qint32 getNewsDataLength();
    Q_INVOKABLE NewsDataItem* getNewsDataFromArrayIndex(int index);
    Q_INVOKABLE QString getNewsDataLastLocalUpdateString();

    Q_INVOKABLE qint32 startListNewsData();
    qint32 handleListNewsDataResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startGetNewsDataItem(qint32 index);
    qint32 handleGetNewsDataItem(MessageProtocol* msg);

    Q_INVOKABLE qint32 startChangeNewsDataItem(const qint32 index, const QString header, const QString info);
    qint32 handleChangeNewsDataResponse(MessageProtocol* msg);
    Q_INVOKABLE NewsDataItem* getCurrentEditedItem();

    Q_INVOKABLE qint32 startRemoveNewsDataItem(const qint32 index);
    qint32 handleRemoveNewsDataItemResponse(MessageProtocol* msg);

signals:

public slots:

private:
    QList<NewsDataItem*> m_lNews;
    QMutex               m_mutex;

    qint64 m_stLastLocalUpdateTimeStamp;
    qint64 m_stLastServerUpdateTimeStamp;

    QString       m_editHeader;
    QString       m_editInfo;
    NewsDataItem* m_editItem = NULL;
};

extern cDataNewsDataManager g_DataNewsDataManager;

#endif // CDATANEWSDATAMANAGER_H
