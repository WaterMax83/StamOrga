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


#ifndef CDATAMEETINGINFO_H
#define CDATAMEETINGINFO_H

#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"
#include "acceptmeetinginfo.h"
#include "cdatacommentitem.h"

class cDataMeetingInfo : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDataMeetingInfo(QObject* parent = nullptr);

    qint32 initialize();

    Q_INVOKABLE void resetMeetingInfo();

    Q_INVOKABLE qint32 startLoadMeetingInfo(const qint32 gameIndex, const qint32 type);
    qint32             handleLoadMeetingInfoResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startSaveMeetingInfo(const qint32 gameIndex, const QString when,
                                            const QString where, const QString info,
                                            const qint32 type);
    qint32             handleSaveMeetingInfoResponse(MessageProtocol* msg);

    Q_INVOKABLE qint32 startAcceptMeetingInfo(const qint32 gameIndex, const qint32 accept,
                                              const QString name, const qint32 type,
                                              const qint32 acceptIndex = 0);
    qint32             handAcceptMeetingInfo(MessageProtocol* msg);

    Q_INVOKABLE qint32 startSendNewComment(const qint32 gameIndex, const QString comment,
                                           const qint32 type);
    qint32             handleSendCommentResponse(MessageProtocol* msg);

    Q_INVOKABLE QString when() { return this->m_when; }
    Q_INVOKABLE QString where() { return this->m_where; }
    Q_INVOKABLE QString info() { return this->m_info; }

    void setWhen(QString when) { this->m_when = when; }
    void setWhere(QString where) { this->m_where = where; }
    void setInfo(QString info) { this->m_info = info; }

    Q_INVOKABLE AcceptMeetingInfo* getAcceptInfoFromIndex(qint32 index);
    qint32                         addNewAcceptInfo(AcceptMeetingInfo* info);
    Q_INVOKABLE void               clearAcceptInfoList();
    Q_INVOKABLE qint32 getAcceptedListCount();

    Q_INVOKABLE cDataCommentItem* getCommentFromIndex(qint32 index);
    Q_INVOKABLE void              clearCommentList();
    Q_INVOKABLE qint32 getCommentCount();


private:
    QString                   m_when;
    QString                   m_where;
    QString                   m_info;
    bool                      m_bDataIsReseted;
    QList<AcceptMeetingInfo*> m_acceptInfo;
    QList<cDataCommentItem*>  m_comments;
    QMutex                    m_mutex;
};

extern cDataMeetingInfo* g_DataMeetingInfo;
extern cDataMeetingInfo* g_DataTripInfo;

#endif // CDATAMEETINGINFO_H
