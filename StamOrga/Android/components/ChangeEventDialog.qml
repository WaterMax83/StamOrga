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

import com.watermax.demo 1.0

import "../components" as MyComponents

MyComponents.CustomDialog {
    property int parentWidth : 540
    property int parentHeight : 960

    property alias headerText: changeGameDialog.title

    property alias homeTeam : inputHome.text
    property alias awayTeam : inputAway.text
    property string score
    property alias seasonIndex : spBoxSeasonIndex.value
    property var competitionIndex
    property string competition : "Other"
    property alias date : inputDate.text
    property alias fixedTime : chBoxTimeFixed.checked
    property alias onlyFanclub : chBoxOnlyFanclub.checked
    property var index



    signal acceptedDialog()

    id: changeGameDialog
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)
    modal: true
    focus: true

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        acceptedDialog();
        changeGameDialog.close();
        changeGameDialog.destroy();
    }
    onRejected: {
        changeGameDialog.close()
        changeGameDialog.destroy();
    }

    contentItem: ColumnLayout {
        id: changeGameDialogColumn
        width: changeGameDialog.width
        spacing: 5

        Rectangle {
            implicitWidth: parent.width
            implicitHeight: 30
            Text {
                id: hintHome
                anchors { fill: parent; leftMargin: 14 }
                verticalAlignment: Text.AlignVCenter
                text: "Überschrift 1"
                color: "#707070"
                opacity: inputHome.displayText.length ? 0 : 1
            }

            TextInput {
                id: inputHome
                anchors { fill:parent; leftMargin: 5 }
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: "#505050"
                KeyNavigation.tab: inputAway
            }
        }

        Rectangle {
            implicitWidth: parent.width
            implicitHeight: 30
            Text {
                id: hintAway
                anchors { fill: parent; leftMargin: 14 }
                verticalAlignment: Text.AlignVCenter
                text: "Überschrift 2"
                color: "#707070"
                opacity: inputAway.displayText.length ? 0 : 1
            }

            TextInput {
                id: inputAway
                anchors { fill:parent; leftMargin: 5 }
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: "#505050"
                KeyNavigation.tab: spBoxSeasonIndex
            }
        }

        SpinBox {
            id: spBoxSeasonIndex
            to: 34
            from: 1
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            KeyNavigation.tab: inputDate
        }

        Rectangle {
            implicitWidth: parent.width
            implicitHeight: 30
            Text {
                id: hintDate
                anchors { fill: parent; leftMargin: 14 }
                verticalAlignment: Text.AlignVCenter
                text: "31.12.2017 15:30"
                color: "#707070"
                opacity: inputDate.displayText.length ? 0 : 1
            }

            TextInput {
                id: inputDate
                anchors { fill:parent; leftMargin: 5 }
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: "#505050"
            }
        }

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: text4
                text: qsTr("Terminiert:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                color: "white"
            }
            CheckBox {
                id: chBoxTimeFixed
            }
        }

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

            Text {
                id: text5
                text: qsTr("Nur Fanclub:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                font.pixelSize: 14
                color: "white"
            }
            CheckBox {
                id: chBoxOnlyFanclub
            }
        }
    }
}
