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

#ifdef Q_OS_WIN
#undef STAM_ORGA_VERSION_LINK_WITH_TEXT
#define STAM_ORGA_VERSION_LINK_WITH_TEXT "<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Winx64.%1.7z\">Lade %1</a>\n"
#define STAM_ORGA_VERSION_COPY_WITH_TEXT "<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Winx64.%1.7z\">Kopiere %1</a>\n"
#define STAM_ORGA_VERSION_LINK "https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Winx64.%1.7z"
#define STAM_ORGA_VERSION_SAVE "StamOrga.Winx64.%1.7z"
#endif
#ifdef Q_OS_ANDROID
#undef STAM_ORGA_VERSION_LINK_WITH_TEXT
#define STAM_ORGA_VERSION_LINK_WITH_TEXT "<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Android.%1.apk\">Lade %1</a>\n"
#define STAM_ORGA_VERSION_COPY_WITH_TEXT "<a href=\"https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Android.%1.apk\">Kopiere %1</a>\n"
#define STAM_ORGA_VERSION_LINK "https://github.com/WaterMax83/StamOrga/releases/download/%1/StamOrga.Android.%1.apk"
#define STAM_ORGA_VERSION_SAVE "StamOrga.Android.%1.apk"
#endif

#define SERVER_HOST_ADDRESS "watermax83.ddns.net"

#endif // CONFIG_H
