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
    id: doubleTicketInfoItem
    width: parent.width
    height: doubleTicketInfoColumn.height

    property string imageColor : "grey"
    property int ticketIndex
    property int menuOpen
    property string title
    property string place
    property string reserve
    property bool  isTicketYourOwn: false

    signal clickedItem(var y)

    ColumnLayout {
        id: doubleTicketInfoColumn
        width: parent.width
        spacing: 0

        RowLayout {
            id: rowItemReserved
            width: parent.width
            height: textItemName.height + 5

            Rectangle {
                id: imageItemReserved
                anchors.left: parent.left
                anchors.leftMargin: parent.height
                anchors.verticalCenter: parent.verticalCenter
                width: parent.height / 4 * 2
                height: parent.height / 4 * 2
                radius: width * 0.5
                color: menuOpen ? "blue": "yellow"
            }

            Text {
                id: textItemName
                text: title
                anchors.left: imageItemReserved.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                Layout.alignment: Qt.AlignVCenter
                color: isTicketYourOwn ? "white" : "#B0BEC5"
                font.pixelSize: 16
            }

            Image {
                id: imageInfoItemButton1
                Layout.preferredHeight: parent.height
                Layout.preferredWidth:  parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: textItemName.right
                anchors.leftMargin: textItemName.width > textReservedFor.width ? 35 : 35 + (textReservedFor.width - textItemName.width)
                fillMode: Image.PreserveAspectFit
                source: "../images/info.png"

            }
            MouseArea {
                anchors.fill: imageInfoItemButton1
                onClicked: {
                    var component = Qt.createComponent("../components/AcceptDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                        dialog.headerText = "Information";
                        dialog.parentHeight = mainPaneCurrentGame.height
                        dialog.parentWidth = mainPaneCurrentGame.width
                        dialog.textToAccept = "Die Karte von<br><br><b>" + title + "</b><br><br>befindet sich aktuell bei<br><br> <b>" + place + "</b>";
                        dialog.showCancelButton = false
                        dialog.font.family= txtForFontFamily.font
                        dialog.open();
                    }
                }
            }
            ColorOverlay {
                anchors.fill: imageInfoItemButton1
                source: imageInfoItemButton1
                color: "#536878"
            }

            MouseArea {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: imageItemReserved.left
                anchors.right: imageInfoItemButton1.left
                onClicked: clickedItem();
            }

            Text {
                id: txtForFontFamily
                visible: false
            }

        }

        Text {
            id: textReservedFor
            text: "-> für " + reserve
            anchors.left: parent.left
            anchors.leftMargin: rowItemReserved.height + imageItemReserved.width + 10
            bottomPadding: 5
            Layout.alignment: Qt.AlignVCenter
            color: "#B0BEC5"
            font.pixelSize: 16
        }

        MouseArea {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: rowItemReserved.left
            anchors.right: textReservedFor.right
            onClicked: {
                var globalCoordinates = doubleTicketInfoColumn.mapToItem(doubleTicketInfoItem.parent, 0, 0)
                clickedItem(globalCoordinates.y - doubleTicketInfoColumn.height / 2)
            }
        }
    }
}
