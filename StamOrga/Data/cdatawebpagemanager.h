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

#ifndef CDATAWEBPAGEMANAGER_H
#define CDATAWEBPAGEMANAGER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "textdataitem.h"

class cDataWebPageManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataWebPageManager(QObject* parent = nullptr);

    qint32 initialize();

    void addNewWebPageData(TextDataItem* sWebPage, const quint16 updateIndex = 0);

    TextDataItem* getWebDataItem(qint32 index);

    bool setWebPageItemHasEvent(qint32 index);
    void resetAllWebPageEvents();

    Q_INVOKABLE qint32 getWebListLength();
    Q_INVOKABLE TextDataItem* getWebDataFromArrayIndex(int index);
    Q_INVOKABLE QString getWebPageLastLocalUpdateString();

    Q_INVOKABLE qint32 startListWebPageData();

    qint32      startAddWebPage();
    Q_INVOKABLE qint32 startLoadWebPage(const qint32 index, const qint32 width);
    qint32 startSetWebPage(const QString text, const QString link);

    qint32 handleWebPageResponse(MessageProtocol* msg);

    //    Q_INVOKABLE qint32 startGetNewsDataItem(qint32 index);
    //    qint32             handleGetNewsDataItem(MessageProtocol* msg);

    //    Q_INVOKABLE qint32 startChangeNewsDataItem(const qint32 index, const QString header, const QString info);
    //    qint32             handleChangeNewsDataResponse(MessageProtocol* msg);
    Q_INVOKABLE TextDataItem* getCurrentEditedItem();

    //    Q_INVOKABLE qint32 startRemoveNewsDataItem(const qint32 index);
    //    qint32             handleRemoveNewsDataItemResponse(MessageProtocol* msg);

signals:

public slots:

private:
    QList<TextDataItem*> m_lWebPages;
    QMutex               m_mutex;

    qint64 m_stLastLocalUpdateTimeStamp;
    qint64 m_stLastServerUpdateTimeStamp;
    qint32 m_screenWidth;

    //    QString       m_editHeader;
    //    QString       m_editInfo;
    TextDataItem* m_editItem = NULL;
};

extern cDataWebPageManager* g_DataWebPageManager;

#endif // CDATAWEBPAGEMANAGER_H
