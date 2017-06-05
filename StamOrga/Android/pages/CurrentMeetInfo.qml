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
//        padding: 0

        ColumnLayout {
            id: mainColumnLayoutCurrentMeetInfo
            width: parent.width

            RowLayout {
                width: parent.width
                spacing: 20

                MyComponents.GraphicalButton {
                    imageSource: "../images/edit.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        isEditMode = true;
                    }
                    Layout.alignment: Qt.AlignLeft
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/save.png"
                    enabled: isEditMode ? true : false
                    onClickedButton: {
                        isEditMode = false;
                        var result = userIntCurrentGame.startSaveMeetingInfo(m_gamePlayCurrentItem.index, textWhen.input, textWhere.input, textInfo.text);
                        if (result === 0)
                            toastManager.show("Keine Änderung, nichts gespeichert", 2000);
                        else {
                            busyLoadingIndicatorCurrentGames.visible = true;
                            txtInfoCurrentGame.visible = true
                            txtInfoCurrentGame.text = "Speichere Infos"
                        }

                    }
                    Layout.alignment: Qt.AlignRight
                }
            }

//            Button {
//                text: "Test"

//            }

            MyComponents.EditableTextWithHint {
                id: textWhere
                hint: "Wo"
                imageSource: "../images/place.png"
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.EditableTextWithHint {
                id: textWhen
                hint: "Wann"
                imageSource: "../images/time.png"
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                onTextInputChanged: checkNewTextInput()
            }

            RowLayout {
                width: parent.width

                MouseArea {
                    width: clipInfoUpImage.width
                    height: clipInfoUpImage.height
                    Image {
                        id: clipInfoUpImage
                        source: "../images/play.png"
                        rotation: isInfoVisible ? 90 : 0
                    }
                    onClicked: {
                        if (isInfoVisible)
                            isInfoVisible = false
                        else
                            isInfoVisible = true;
                    }
                }

                Label {
                    text: "Info"
                }

                ToolSeparator {
                    id: toolSeparator1
                    orientation: "Horizontal"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                implicitWidth: parent.width
                implicitHeight: textInfo.height > 0 ? textInfo.height : 30
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                visible: isInfoVisible
                Text {
                    id: hintTextInfo
                    anchors { fill: parent; leftMargin: 14 }
                    verticalAlignment: Text.AlignVCenter
                    color: "#808080"
                    text: "Info"
                    opacity: textInfo.text.length ? 0 : 1
                }
                TextArea {
                    id: textInfo
                    width: parent.width
                    color: "#505050"
                    leftPadding: 5
                    enabled: isEditMode
                    onTextChanged: checkNewTextInput()
                }
            }

        }
    }

    function toolButtonClicked() {

    }

    property var  meetingInfo;
    property bool isEditMode: false
    property bool isInputAlreadyChanged: false
    property bool isInfoVisible: false

    function showAllInfoAboutGame() {
        meetingInfo = globalUserData.getMeetingInfo();
        textInfo.text = meetingInfo.info();
        textWhen.init(meetingInfo.when())
        textWhere.init(meetingInfo.where())
    }

    function checkNewTextInput() {
        if (isInputAlreadyChanged)
            return;
        isInputAlreadyChanged = true;
    }

    function notifyChangedMeetingInfoFinished(result) {
        if (result === 1) {
            toastManager.show("Info erfolgreich gespeichert", 2000);
//            loadAvailableTicketList()

        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);

        }
        busyLoadingIndicatorCurrentGames.visible = false;
        txtInfoCurrentGame.visible = false
    }

}



