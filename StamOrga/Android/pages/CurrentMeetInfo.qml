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

import "../components" as MyComponents

Flickable {
    id: flickableCurrentMeetInfo
    height: parent.height * 1.2
    contentHeight: mainPaneCurrentMeetInfo.height

    flickableDirection: Flickable.VerticalFlick
    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    property int listViewItemHeight : 30



    onDragEnded: {
        if (flickableCurrentMeetInfo.contentY < -100) {
//            loadAvailableTicketList();
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableCurrentMeetInfo.contentY > -100) ? (flickableCurrentMeetInfo.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableCurrentMeetInfo.contentY * -1) / 100
        color: "black"
        width: refreshImage.width
        height: refreshImage.height
        radius: width * 0.5
        y: 50
        x: (mainWindow.width / 2) - (width / 2)
        z: 1000
    }

    Pane {
        id: mainPaneCurrentMeetInfo
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutCurrentMeetInfo
            width: parent.width

            MyComponents.EditableTextWithHint {
                id: whenText
                hint: "Wann"
                width: parent.width
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.EditableTextWithHint {
                id: whereText
                hint: "Wo"
                width: parent.width
                onTextInputChanged: checkNewTextInput()
            }

        }
    }

    property bool isInputAlreadyChanged: false

    function showAllInfoAboutGame(sender) {
//          whenText.init("hallo");
    }

    function checkNewTextInput() {
        if (isInputAlreadyChanged)
            return;
        isInputAlreadyChanged = true;
        updateHeaderFromMain("Hallo Welt", "")
    }
}
