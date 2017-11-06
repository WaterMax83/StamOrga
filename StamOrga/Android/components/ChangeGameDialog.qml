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
    property alias score : inputScore.text
    property alias seasonIndex : spBoxSeasonIndex.value
    property alias competitionIndex : comboBoxCompetition.currentIndex
    property alias competition : comboBoxCompetition.currentText
    property alias date : inputDate.text
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
                text: "Heimteam"
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
                text: "Auswärtsteam"
                color: "#707070"
                opacity: inputAway.displayText.length ? 0 : 1
            }

            TextInput {
                id: inputAway
                anchors { fill:parent; leftMargin: 5 }
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
                color: "#505050"
                KeyNavigation.tab: inputScore
            }
        }

        Rectangle {
            implicitWidth: parent.width
            implicitHeight: 30
            Text {
                id: hintScore
                anchors { fill: parent; leftMargin: 14 }
                verticalAlignment: Text.AlignVCenter
                text: "0:0"
                color: "#707070"
                opacity: inputScore.displayText.length ? 0 : 1
            }

            TextInput {
                id: inputScore
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
            KeyNavigation.tab: comboBoxCompetition
        }

        ComboBox {
            id: comboBoxCompetition
            implicitWidth: parent.width
            model: ["1.Bundesliga", "2.Bundesliga", "3.Liga", "DFB Pokal", "Badischer Pokal", "TestSpiel"]
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
    }
}
