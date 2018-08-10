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

import com.watermax.demo 1.0

Item {
    id: singleTicketInfoItem
    width: parent.width
    height: singleTicketInfoRow.height
    property string imageColor : "grey"
    property int ticketIndex
    property int menuOpen
    property string title
    property string place
    property string timestamp
    property bool  isTicketYourOwn: false

    signal clickedItem(var y)

    RowLayout {
        id: singleTicketInfoRow
        width: parent.width
        height: textItemName.height + 5

        Rectangle {
            id: imageItemInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: parent.height
            width: parent.height / 4 * 2
            height: parent.height / 4 * 2
            radius: width * 0.5
            color: menuOpen ? "blue": imageColor
        }

        Text {
            id: textItemName
            text: title
            anchors.left: imageItemInfo.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            Layout.alignment: Qt.AlignVCenter
            color: isTicketYourOwn ? "white" : "#B0BEC5"
            font.pixelSize: 16
        }

        Image {
            id: imageInfoItemButton
            Layout.preferredHeight: parent.height
            Layout.preferredWidth:  parent.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            fillMode: Image.PreserveAspectFit
            source: "../images/info.png"
        }
        ColorOverlay {
            anchors.fill: imageInfoItemButton
            source: imageInfoItemButton
            color: "#536878"
        }

        MouseArea {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: imageInfoItemButton.left
            onClicked: {
                var globalCoordinates = singleTicketInfoRow.mapToItem(singleTicketInfoItem.parent, 0, 0)
                clickedItem(globalCoordinates.y - singleTicketInfoRow.height / 2)
            }
        }

        Text {
            id: txtForFontFamily
            visible: false
        }

        MouseArea {
            anchors.fill: imageInfoItemButton
            onClicked: {
                var component = Qt.createComponent("../components/AcceptDialog.qml");
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                    dialog.headerText = "Information";
                    dialog.parentHeight = mainPaneCurrentGame.height
                    dialog.parentWidth = mainPaneCurrentGame.width
                    dialog.textToAccept = "Die Karte von<br><br><b>" + title + "</b><br><br>
                                            befindet sich seit<br><br><b>" + timestamp + "</b><br><br>
                                            aktuell bei<br><br> <b>" + place + "</b>";
                    dialog.showCancelButton = false
                    dialog.font.family= txtForFontFamily.font
                    dialog.open();
                }
            }
        }
    }
}
