/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Extras 1.4
import QtQuick.Window 2.2

import "../components" as MyComponents

Rectangle {
    id: mainRectangle
    property var m_gamePlayItem

    width: 200
    height: childrenRect.height * 1.1
    radius: 8
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#275d58"
        }

        GradientStop {
            position: 0.51
            color: "#85c4be"
        }
    }
    border.color: "grey"
    border.width: 2
    ColumnLayout {
        id: columnLayout
        width: mainRectangle.width
        spacing: 1
        Label {
            id: labelLine1
            text: qsTr("Date")
            leftPadding: 5
            topPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        Label {
            id: labelLine2
            text: qsTr("What")
            font.letterSpacing: 2
            fontSizeMode: Text.FixedSize
            leftPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        RowLayout {
            id: layoutExplicitGame
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayout.width
            Layout.fillWidth: true
            Label {
                id: labelLineHome
                width: 36
                text: qsTr("Home")
                leftPadding: 5
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Button {
                id: button
                text: qsTr("Button")
            }

            Label {
                id: labelLineHyphen
                width: contentWidth
                text: qsTr(" - ")
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineAway
                width: contentWidth
                text: qsTr("Away")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: labelLineScore
                width: contentWidth
                text: qsTr("0:0")
                rightPadding: 20
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }

        }
    }
}
