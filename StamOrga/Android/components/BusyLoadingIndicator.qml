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

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

import com.watermax.demo 1.0

ColumnLayout {
    property alias loadingVisible : busyLoadingIndicator.visible
    property alias infoText: txtInfo.text
    property alias infoVisible: txtInfo.visible
    property alias infoHeight: txtInfo.height

    spacing: 0
    width: parent.width
    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

    BusyIndicator {
        id: busyLoadingIndicator
        visible: false
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }

    Text {
        id: txtInfo
        visible: false
        color: "white"
        font.pixelSize: 12
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
}


