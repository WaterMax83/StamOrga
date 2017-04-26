/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "configlist.h"

ConfigList::ConfigList()
{

}


quint32 ConfigList::getNextInternalIndex()
{
    quint32 savedIndex;

    QMutexLocker locker(&this->m_mConfigIniMutex);

    this->m_pConfigSettings->beginGroup(ITEM_INDEX_GROUP);
    savedIndex = this->m_pConfigSettings->value(ITEM_MAX_INDEX, 0).toUInt();

    savedIndex++;

    this->m_pConfigSettings->setValue(ITEM_MAX_INDEX, savedIndex);
    this->m_pConfigSettings->endGroup();

    return savedIndex;
}
