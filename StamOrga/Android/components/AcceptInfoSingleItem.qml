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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material 2.2

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    id: singleRowAcceptItem
    width: parent.width
    height: singleRowAccepted.height

    property color imageColor: "grey"
    property string imageSource: ""
    property string infoTitle: ""
    property var infoValue
    property var infoIndex

    signal clickedElement(var y);
    RowLayout {
        id: singleRowAccepted
        width: parent.width
        height: 25

        Rectangle {
            id: imageItemAccepted
            anchors.left: parent.left
            anchors.leftMargin: parent.height
            width: parent.height / 4 * 3
            height: parent.height / 4 * 3
            radius: width * 0.5
            color: imageColor
            Image {
                anchors.fill: parent
                source: imageSource
            }
        }

        Text {
            id: textItemAccepted
            text: infoTitle
            anchors.left: imageItemAccepted.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            Layout.alignment: Qt.AlignVCenter
            color: "white"
            font.pixelSize: 16
        }
        MouseArea {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: imageItemAccepted.left
            anchors.right: textItemAccepted.right
            onClicked: {
                var globalCoordinates = singleRowAccepted.mapToItem(singleRowAcceptItem.parent, 0, 0)
                clickedElement(globalCoordinates.y - singleRowAccepted.height / 2)
            }
        }
    }

}
