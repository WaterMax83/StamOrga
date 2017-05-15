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

#include <QtCore/QDebug>

#include "gameplay.h"

GamePlay::GamePlay(QObject* parent)
    : QObject(parent)
{
}


bool GamePlay::isGameInPast()
{
    QDateTime now = QDateTime::currentDateTime();
    if (now.toMSecsSinceEpoch() > this->m_timestamp + (2 * 60 * 60 * qint64(1000)))
        return true;

    return false;
}

bool GamePlay::isGameRunning()
{
    QDateTime now = QDateTime::currentDateTime();
    if (now.toMSecsSinceEpoch() > this->m_timestamp) {
        if (now.toMSecsSinceEpoch() < this->m_timestamp + (2 * 60 * 60 * qint64(1000)))
            return true;
    }
    return false;
}
