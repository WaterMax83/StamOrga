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


#ifndef CDATAPPINFOMANAGER_H
#define CDATAPPINFOMANAGER_H

#include <QObject>

#include "../Common/General/cgendisposer.h"

class cDatAppInfoManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cDatAppInfoManager(QObject* parent = nullptr);

    qint32 initialize() override;

    QString getCurrentAppGUID();
    QString getCurrentAppToken();

signals:

public slots:

private:
    QString m_pushNotificationToken;
    QString m_AppInstanceGUID;
};

extern cDatAppInfoManager g_DatAppInfoManager;

#endif // CDATAPPINFOMANAGER_H
