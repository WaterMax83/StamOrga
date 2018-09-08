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

#ifndef CDATAMEDIAMANAGER_H
#define CDATAMEDIAMANAGER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QMutex>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

struct MediaContent {
    QString    m_fileUrl;
    QByteArray m_data;
};

class cDataMediaManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataMediaManager(QObject* parent = nullptr);

    qint32 initialize() override;

    Q_INVOKABLE qint32 startGetPictureList(const qint32 gameIndex);
    Q_INVOKABLE qint32 startAddPicture(const qint32 gameIndex, QString url);
    Q_INVOKABLE qint32 startDeletePictures(const qint32 gameIndex, QStringList list);
    qint32             handleMediaCommandResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 getMediaCount();
    Q_INVOKABLE QString getMediaFileString(qint32 index);

signals:

public slots:

private:
    QMutex               m_mutex;
    QList<MediaContent*> m_files;
};

extern cDataMediaManager* g_DataMediaManager;
#endif // CDATAMEDIAMANAGER_H
