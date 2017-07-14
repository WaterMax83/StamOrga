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

#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/QString>

//#define STAM_ORGA_VERSION_I 0x01000000 // VX.Y.Z => 0xXXYYZZBB
//#define STAM_ORGA_VERSION_S "V1.0.0"

#define STAM_ORGA_VERSION_S "V" + QString(STAMORGA_VERSION)
#define STAM_ORGA_VERSION_I QString("0x0" + QString(STAMORGA_VERSION).replace(".", "0") + "00").toUInt(NULL, 16)

#endif // CONFIG_H
