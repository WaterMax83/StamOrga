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

    GridLayout {
        id: doubleTicketInfoColumn
        width: parent.width
//        spacing: 0
        rows: 2
        columns: 2

        RowLayout {
            id: rowItemReserved
//            width: parent.width
//            height: textItemName.height + 5
            Layout.column: 0
            Layout.row: 0

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

            Text {
                id: txtForFontFamily
                visible: false
            }

        }
        RowLayout {
            id: rowItemReserved2
            //            width: parent.width
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: textReservedFor.height + 5
            Layout.column: 0
            Layout.row: 1

            Text {
                id: textReservedFor
                text: "-> für " + reserve
                anchors.left: parent.left
                anchors.leftMargin: height + imageItemReserved.width + 10
                Layout.alignment: Qt.AlignVCenter
                color: "#B0BEC5"
                font.pixelSize: 16
            }


        }

        Item {
            id: imageItemForInfo
            width: textReservedFor.height + 5
            height: textReservedFor.height + 5
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            Layout.column: 1
            Layout.rowSpan: 2
            Image {
                id: imageInfoItemButton
                sourceSize: Qt.size(parent.width, parent.height)
                fillMode: Image.PreserveAspectFit
                Layout.alignment: Qt.AlignRight
                source: "../images/info.png"

            }

            ColorOverlay {
                anchors.fill: imageInfoItemButton
                source: imageInfoItemButton
                color: "#536878"
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
                        dialog.textToAccept = "Die Karte von<br><br><b>" + title + "</b><br><br>befindet sich aktuell bei<br><br> <b>" + place + "</b>";
                        dialog.showCancelButton = false
                        dialog.font.family= txtForFontFamily.font
                        dialog.open();
                    }
                }
            }
        }

        MouseArea {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: imageItemForInfo.left
            onClicked: {
                var globalCoordinates = doubleTicketInfoColumn.mapToItem(doubleTicketInfoItem.parent, 0, 0)
                clickedItem(globalCoordinates.y - doubleTicketInfoColumn.height / 2)
            }
        }
    }
}
