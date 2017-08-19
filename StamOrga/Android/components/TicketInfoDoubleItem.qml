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

import com.watermax.demo 1.0


ColumnLayout {
    width: parent.width
    spacing: 0
    property var itemModel

    signal clickedItem()

    RowLayout {
        id: rowItemReserved
        width: parent.width
        height: listViewItemHeight
        anchors.left: parent.left
        anchors.leftMargin: listViewItemHeight


        Rectangle {
            id: imageItemReserved
            anchors.left: parent.left
            width: parent.height / 4 * 2
            height: parent.height / 4 * 2
            radius: width * 0.5
            color: itemModel.menuOpen ? "blue": "yellow"
        }

        Text {
            id: textItemName
            text: itemModel.title
            anchors.left: imageItemReserved.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            Layout.alignment: Qt.AlignVCenter
            color: "white"
            font.pixelSize: listViewItemHeight / 5 * 2
        }

        Image {
            id: imageInfoItemButton
            height: textItemName.height
            width: textItemName.height
            anchors.left: textItemName.right
            anchors.leftMargin: textItemName.width > textReservedFor.width ? 35 : 35 + (textReservedFor.width - textItemName.width)
            source: "../images/info.png"
        }
        MouseArea {
            anchors.top: imageInfoItemButton.top
            anchors.bottom: imageInfoItemButton.bottom
            anchors.left: imageItemReserved.left
            anchors.right: imageInfoItemButton.right
            onClicked: clickedItem();
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
                    dialog.textToAccept = "Die Karte von<br><br><b>" + itemModel.title + "</b><br><br>befindet sich aktuell bei<br><br> <b>" + itemModel.place + "</b>";
                    dialog.showCancelButton = false
                    dialog.open();
                }
            }
        }
    }

    Text {
        id: textReservedFor
        text: "-> für " + itemModel.reserve
        anchors.left: parent.left
        anchors.leftMargin: listViewItemHeight + imageItemReserved.width + 10
        bottomPadding: 5
        Layout.alignment: Qt.AlignVCenter
        color: "white"
        font.pixelSize: listViewItemHeight / 6 * 2.5
    }

    MouseArea {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: rowItemReserved.left
        anchors.right: textReservedFor.right
        onClicked: clickedItem();
    }
}
