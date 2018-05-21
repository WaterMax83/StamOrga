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

#ifndef CDATACOMMENTITEM_H
#define CDATACOMMENTITEM_H

#include <QObject>

class cDataCommentItem : public QObject
{
    Q_OBJECT
public:
    explicit cDataCommentItem(QObject* parent = nullptr);

    void setUser(const QString user);
    Q_INVOKABLE QString getUser();

    void setTimeStamp(const qint64 timeStamp);
    Q_INVOKABLE QString getReadableTime();

    void setComment(const QString comment);
    Q_INVOKABLE QString getComment();

private:
    QString m_user;
    qint64  m_timestamp;
    QString m_comment;
};

#endif // CDATACOMMENTITEM_H
