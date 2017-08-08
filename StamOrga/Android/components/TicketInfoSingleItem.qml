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

RowLayout {
    width: parent.width
    property var itemModel
    property var imageColor

    signal clickedItem()

    Rectangle {
        id: imageItemInfo
        anchors.left: parent.left
        anchors.leftMargin: parent.height
        width: parent.height / 4 * 2
        height: parent.height / 4 * 2
        radius: width * 0.5
        color: itemModel.menuOpen ? "blue": imageColor
    }

    Text {
        id: textItemName
        text: itemModel.title
        anchors.left: imageItemInfo.right
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        Layout.alignment: Qt.AlignVCenter
        color: "white"
        font.pixelSize: parent.height / 4 * 2
    }

    Image {
        id: imageInfoItemButton
        height: parent.height
        width: parent.height
        anchors.left: textItemName.right
        anchors.leftMargin: 35
        source: "../images/info.png"
    }

    MouseArea {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: imageItemInfo.left
        anchors.right: textItemName.right
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
