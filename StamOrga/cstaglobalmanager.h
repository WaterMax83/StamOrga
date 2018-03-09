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

#ifndef CSTAGLOBALMANAGER_H
#define CSTAGLOBALMANAGER_H

#include <QObject>

#include "../Common/General/cgendisposer.h"


class cStaGlobalManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cStaGlobalManager(QObject* parent = nullptr);

    qint32 initialize();

signals:

public slots:
};

#endif // CSTAGLOBALMANAGER_H
