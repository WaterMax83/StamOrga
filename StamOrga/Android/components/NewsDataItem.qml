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

Rectangle {
    id: newsRectangleItem
    property var m_newsDataItem

    signal clickedItem(var sender)
    signal pressAndHold(var sender)

    width: parent.width
    height: childrenRect.height * 1.1
    property color gradColorStart: "#003333"
    property color gradColorStop: "#008080"
    gradient: Gradient {
        GradientStop {
            position: 0
            color: gradColorStart
        }

        GradientStop {
            position: 0.5
            color: gradColorStop
        }
    }
    radius: 5
    border.color: "grey"
    border.width: 2

    ColumnLayout {
        id: columnLayoutNewsItem
        width: parent.width
        spacing: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                newsRectangleItem.clickedItem(m_newsDataItem);
            }
            onPressAndHold: {
                newsRectangleItem.pressAndHold(m_newsDataItem);
            }
        }
        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: labelHeaderItem
                text: qsTr("")
                leftPadding: 10
                topPadding: 5
                color: "white"
                font.pixelSize: 16
                font.bold: true
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }


        }

        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: labelUserName
                text: qsTr("")

                leftPadding: 10
                visible: true
                color: "white"
                font.pixelSize: 14
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelTime
                text: qsTr("")

                rightPadding: 10
                visible: true
                color: "white"
                font.pixelSize: 14
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }

    }

    function showNewsDataInfo(index) {
        var newsDataItem = globalUserData.getNewsDataItemFromArrayIndex(index)
        if (newsDataItem !== null) {
            m_newsDataItem = newsDataItem
            labelHeaderItem.text = newsDataItem.header
            labelUserName.text = "von " + newsDataItem.user;
            labelTime.text = newsDataItem.timestampReadableLine();
        }
    }
}
