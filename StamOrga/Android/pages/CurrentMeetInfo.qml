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

    signal showInfoHeader(var text, var load);

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
            showInfoHeader("Aktualisiere Daten", true)
            userIntCurrentGame.startLoadMeetingInfo(m_gamePlayCurrentItem.index);
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
                Layout.fillWidth: true

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
                            showInfoHeader("Speichere Infos", false)
                        }

                    }
                    Layout.alignment: Qt.AlignRight
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/done.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        acceptValue = 1;
                        showTextDialogAccept("Zusagen");
                    }
                    Layout.alignment: Qt.AlignRight
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/help.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        acceptValue = 2;
                        showTextDialogAccept("Interesse/Vorbehalt");
                    }
                    Layout.alignment: Qt.AlignRight
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/close.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        acceptValue = 3;
                        showTextDialogAccept("Absagen");
                    }
                    Layout.alignment: Qt.AlignRight
                }
            }

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

            RowLayout {
                width: parent.width
                MouseArea {
                    width: clipAcceptUpImage.width
                    height: clipAcceptUpImage.height
                    Image {
                        id: clipAcceptUpImage
                        source: "../images/play.png"
                        rotation: isAcceptVisible ? 90 : 0
                    }
                    onClicked: {
                        if (isAcceptVisible)
                            isAcceptVisible = false
                        else
                            isAcceptVisible = true;
                    }
                }
                Label {
                    text: "Zusagen"
                }
                ToolSeparator {
                    id: toolSeparator2
                    orientation: "Horizontal"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: true
                }
            }

            Item {
                width: parent.width
                visible: isAcceptVisible

                ListView {
                    id: listViewAcceptedUsers
                    interactive: false
                    implicitWidth: mainColumnLayoutCurrentGame.width

                    delegate: RowLayout {
                        id: singleRowAccepted
                        width: parent.width
                        height: listViewItemHeight

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var globalCoordinates = singleRowAccepted.mapToItem(flickableCurrentMeetInfo, 0, 0)
                                clickedAcceptedUserMenu.y = globalCoordinates.y - singleRowAccepted.height / 2
                                menuAcceptIndex = model.index
                                menuAcceptValue = model.value
                                menuAcceptText = model.title
                                clickedAcceptedUserMenu.open();
                            }
                        }
                        Rectangle {
                            id: imageItemAccepted
                            anchors.left: parent.left
                            anchors.leftMargin: parent.height
                            width: parent.height / 4 * 3
                            height: parent.height / 4 * 3
                            radius: width * 0.5
                            color: model.color
                            Image {
                                anchors.fill: parent
                                source: model.source
                            }
                        }

                        Text {
                            text: model.title
                            anchors.left: imageItemAccepted.right
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            Layout.alignment: Qt.AlignVCenter
                            color: "white"
                            font.pixelSize: parent.height / 4 * 2
                        }
                    }

                    model: ListModel {
                        id: listViewModelAcceptedUsers
                    }
                }
            }
        }
    }

    property int menuAcceptIndex
    property int menuAcceptValue
    property string menuAcceptText


    Menu {
            id: clickedAcceptedUserMenu
            x: (flickableCurrentMeetInfo.width - width) / 2
            y: flickableCurrentMeetInfo.height / 6

            background: Rectangle {
                    implicitWidth: menuItemAccept.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemAccept
                visible: menuAcceptValue !== 1 ? true : false
                height: visible ? implicitHeight : 0
                text: "Zusagen"
                onClicked: {
                    userIntCurrentGame.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 1, menuAcceptText, menuAcceptIndex);
                    showInfoHeader("Ändere Teilnahme", true)
                }
            }

            MenuItem {
                id: menuItemInterest
                text: "Interesse/Vorbehalt"
                visible: menuAcceptValue !== 2 ? true : false
                height: visible ? implicitHeight : 0
                onClicked: {
                    userIntCurrentGame.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 2, menuAcceptText, menuAcceptIndex);
                    showInfoHeader("Ändere Teilnahme", true)
                }
            }

            MenuItem {
                id: menuItemDecline
                text: "Absagen"
                visible: menuAcceptValue !== 3 ? true : false
                height: visible ? implicitHeight : 0
                onClicked: {
                    userIntCurrentGame.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 3, menuAcceptText, menuAcceptIndex);
                    showInfoHeader("Ändere Teilnahme", true)
                }
            }

            MenuItem {
                id: menuItemEdit
                text: "Ändern"
                onClicked: {
                    //                    var component = Qt.createComponent("../components/EditableTextDialog.qml");
                    //                    if (component.status === Component.Ready) {
                    //                        var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                    //                        dialog.headerText = "Reserviere für";
                    //                        dialog.parentHeight = mainWindow.height
                    //                        dialog.parentWidth =  mainPaneCurrentGame.width
                    //                        dialog.textMinSize = 5;
                    //                        dialog.acceptedTextEdit.connect(acceptedEditReserveNameDialog);
                    //                        dialog.open();
                    //                    }
                }
            }
        }

    function showTextDialogAccept(header) {
        var component = Qt.createComponent("../components/EditableTextDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
            dialog.headerText = header;
            dialog.parentHeight = mainWindow.height
            dialog.parentWidth =  mainPaneCurrentGame.width
            dialog.textMinSize = 5;
            dialog.acceptedTextEdit.connect(acceptedEditTextDialogAccept);
            dialog.open();
        }
    }

    function acceptedEditTextDialogAccept(text) {
        userIntCurrentGame.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, acceptValue, text);
        showInfoHeader("Ändere Teilnahme", true)
    }

    function toolButtonClicked() {

    }

    property int  acceptValue;
    property var  meetingInfo;
    property bool isEditMode: false
    property bool isInputAlreadyChanged: false
    property bool isInfoVisible: false
    property bool isAcceptVisible: false

    function showAllInfoAboutGame() {
        meetingInfo = globalUserData.getMeetingInfo();
        loadMeetingInfo();
    }

    function checkNewTextInput() {
        if (isInputAlreadyChanged)
            return;
        isInputAlreadyChanged = true;
    }

    function notifyChangedMeetingInfoFinished(result) {
        if (result === 1) {
            toastManager.show("Info erfolgreich gespeichert", 2000);
            loadMeetingInfo();
        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader("Infos speichern hat nicht funktioniert", false)
        }
    }

    function notifyLoadMeetingInfoFinished(result) {
        if (result === 1) {
            toastManager.show("Info übers Treffen geladen", 2000);
            textInfo.text = meetingInfo.info();
            textWhen.init(meetingInfo.when())
            textWhere.init(meetingInfo.where())
            showInfoHeader("", false)
        } else if (result === -5) {
            toastManager.show("Bisher noch kein Treffen gespeichert", 2000);
            textInfo.text = "";
            textWhen.init("")
            textWhere.init("")
            showInfoHeader("", false)
        } else {
            showInfoHeader("Infos laden hat nicht funktioniert", false)
        }

        listViewModelAcceptedUsers.clear();

        if (result === 1 && meetingInfo.getAcceptedListCount() > 0) {
            for (var i = 0; i < meetingInfo.getAcceptedListCount(); i++) {
                var acceptInfo = meetingInfo.getAcceptInfoFromIndex(i)
                var btnColor;
                var imgSource;
                if (acceptInfo.value() === 1) {
                    btnColor = "green";
                    imgSource = "../images/done.png";
                }
                else if (acceptInfo.value() === 2) {
                    btnColor = "orange";
                    imgSource = "../images/help.png";
                }
                else if (acceptInfo.value() === 3) {
                    btnColor = "red";
                    imgSource = "../images/close.png";
                }
                listViewModelAcceptedUsers.append({
                                                    title: acceptInfo.name(),
                                                    index: acceptInfo.index(),
                                                    color: btnColor,
                                                    source: imgSource,
                                                    value: acceptInfo.value()
                                                });

            }
            /* Does not work in defintion for freeTickets, so set it here */
            listViewAcceptedUsers.implicitHeight = listViewModelAcceptedUsers.count * listViewItemHeight

        } else {
            listViewAcceptedUsers.implicitHeight = 0;
        }
    }

    function notifyAcceptMeetingFinished(result) {
        if (result === 1) {
            toastManager.show("Teilnahme erfolgreich geändert", 2000);
            loadMeetingInfo();

        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader("Teilnehmen hat nicht funktioniert", false)
        }
    }

    function loadMeetingInfo()
    {
        listViewModelAcceptedUsers.clear();
        meetingInfo.clearAcceptInfoList();
        showInfoHeader("Aktualisiere Daten", true)
        userIntCurrentGame.startLoadMeetingInfo(m_gamePlayCurrentItem.index);
    }

}



