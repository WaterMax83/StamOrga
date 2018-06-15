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

//import QtQuick.Controls.Material 2.2

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    width: parent.width
    height: rowSeperator.height

    property bool isShiftVisible: false
    property bool userClosedShift : false
    property string textToShow : ""
    RowLayout {
        id: rowSeperator
        width: parent.width
        MouseArea {
            width: clipUpImage.width
            height: clipUpImage.height
            Image {
                id: clipUpImage
                source: "../images/play.png"
                rotation: isShiftVisible ? 90 : 0
                width: 25
                height: 25
            }
            ColorOverlay {
                anchors.fill: clipUpImage
                source: clipUpImage
                color: "grey"
                rotation: isShiftVisible ? 90 : 0
            }
            onClicked: {
                if (isShiftVisible) {
                    isShiftVisible = false
                    userClosedShift = true;
                } else
                    isShiftVisible = true;
            }
        }
        Text {
            id: txtForShowing
            color: "grey"
            text: textToShow
            font.pixelSize: 14
        }
        ToolSeparator {
            id: toolSeparator3
            orientation: "Horizontal"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
        }
    }

}
