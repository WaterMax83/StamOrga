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

Flickable {
    id: flickableCurrentMeetInfo
    clip: true
    height: parent.height * 1.2
    contentHeight: mainPaneCurrentMeetInfo.height
    property int meetingType: 0
    property bool useCommentLine : true

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
        if (flickableCurrentMeetInfo.contentY < -75) {
            lDataMeetingInfo.clearCommentList();
            deleteValues();
            showInfoHeader("Aktualisiere Daten", true)
            lDataMeetingInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableCurrentMeetInfo.contentY > -75) ? (flickableCurrentMeetInfo.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableCurrentMeetInfo.contentY * -1) / 75
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

            Text {
                id: txtForFontFamily
                visible: false
            }

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
                        var result = lDataMeetingInfo.startSaveMeetingInfo(m_gamePlayCurrentItem.index, textWhen.input, textWhere.input,
                                                                           textInfo.text, meetingType);
                        if (result === 0)
                            toastManager.show("Keine Änderung, nichts gespeichert", 2000);
                        else {
                            showInfoHeader("Speichere Infos", true)
                        }

                    }
                    Layout.alignment: Qt.AlignRight
                }

                Text {
                    text: " - "
                    font.bold: true
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/done.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        menuAcceptValue = 1;
                        menuAcceptIndex = 0;
                        //                        if (globalSettings.useReadableName)
                        showTextDialogAccept("Zusagen", gConUserSettings.getReadableName());
                        //                        else
                        //                            showTextDialogAccept("Zusagen", "");
                    }
                    Layout.alignment: Qt.AlignRight
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/help.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        menuAcceptValue = 2;
                        menuAcceptIndex = 0;
                        //                        if (globalSettings.useReadableName)
                        showTextDialogAccept("Interesse/Vorbehalt", gConUserSettings.getReadableName());
                        //                        else
                        //                            showTextDialogAccept("Interesse/Vorbehalt", "");
                    }
                    Layout.alignment: Qt.AlignRight
                }

                MyComponents.GraphicalButton {
                    imageSource: "../images/close.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        menuAcceptValue = 3;
                        menuAcceptIndex = 0;
                        //                        if (globalSettings.useReadableName)
                        showTextDialogAccept("Absagen", gConUserSettings.getReadableName());
                        //                        else
                        //                            showTextDialogAccept("Absagen", "");
                    }
                    Layout.alignment: Qt.AlignRight
                }
            }

            MyComponents.EditableTextWithHint {
                id: textWhere
                hint: "Wo"
                imageSource: meetingType === 0 ? "../images/place.png" : "../images/train.png"
                enableImage: true
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.EditableTextWithHint {
                id: textWhen
                hint: "Wann"
                imageSource: "../images/time.png"
                enableImage: true
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.ShiftableSeperator {
                id: infoSeperator
                width: parent.width
                textToShow: "Info"
            }

            Rectangle {
                implicitWidth: parent.width
                implicitHeight: textInfo.height > 0 ? textInfo.height : listViewItemHeight
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                visible: infoSeperator.isShiftVisible
                //                Text {
                //                    id: hintTextInfo
                //                    anchors { fill: parent; leftMargin: 14 }
                //                    verticalAlignment: Text.AlignVCenter
                //                    color: "#808080"
                //                    text: "Info"
                //                    opacity: textInfo.text.length ? 0 : 1
                //                }
                TextArea {
                    id: textInfo
                    font.family: txtForFontFamily.font
                    width: parent.width
                    color: "#505050"
                    placeholderText: "Information"
                    leftPadding: 5
                    font.pixelSize: 16
                    enabled: isEditMode
                    onTextChanged: checkNewTextInput()
                }
            }

            MyComponents.ShiftableSeperator {
                id: acceptSeperator
                width: parent.width
                textToShow: "Zusagen"
            }

            Column {
                id: columnLayoutAccepts
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                visible: acceptSeperator.isShiftVisible
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            //            ListView {
            //                id: listViewAcceptedUsers
            //                visible: acceptSeperator.isShiftVisible
            //                interactive: false
            //                //                implicitWidth: mainColumnLayoutCurrentGame.width
            //                implicitWidth: parent.width

            //                delegate: RowLayout {
            //                    id: singleRowAccepted
            //                    width: parent.width
            //                    height: listViewItemHeight

            //                    Rectangle {
            //                        id: imageItemAccepted
            //                        anchors.left: parent.left
            //                        anchors.leftMargin: parent.height
            //                        width: parent.height / 4 * 3
            //                        height: parent.height / 4 * 3
            //                        radius: width * 0.5
            //                        color: model.color
            //                        Image {
            //                            anchors.fill: parent
            //                            source: model.source
            //                        }
            //                    }

            //                    Text {
            //                        id: textItemAccepted
            //                        text: model.title
            //                        anchors.left: imageItemAccepted.right
            //                        anchors.leftMargin: 10
            //                        anchors.verticalCenter: parent.verticalCenter
            //                        Layout.alignment: Qt.AlignVCenter
            //                        color: "white"
            //                        font.pixelSize: parent.height / 5 * 3
            //                    }
            //                    MouseArea {
            //                        anchors.top: parent.top
            //                        anchors.bottom: parent.bottom
            //                        anchors.left: imageItemAccepted.left
            //                        anchors.right: textItemAccepted.right
            //                        onClicked: {
            //                            var globalCoordinates = singleRowAccepted.mapToItem(flickableCurrentMeetInfo, 0, 0)
            //                            clickedAcceptedUserMenu.y = globalCoordinates.y - singleRowAccepted.height / 2
            //                            menuAcceptIndex = model.index
            //                            menuAcceptValue = model.value
            //                            menuAcceptText = model.title
            //                            clickedAcceptedUserMenu.open();
            //                        }
            //                    }
            //                }

            //                model: ListModel {
            //                    id: listViewModelAcceptedUsers
            //                }
            //            }

            MyComponents.ShiftableSeperator {
                id: commentSeperator
                width: parent.width
                textToShow: "Kommentare"
            }

            Column {
                id: columnLayoutComments
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                visible: commentSeperator.isShiftVisible
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {
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
            font.family: txtForFontFamily.font
            visible: menuAcceptValue !== 1 ? true : false
            height: visible ? implicitHeight : 0
            text: "Zusagen"
            onClicked: {
                lDataMeetingInfo.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 1, menuAcceptText, meetingType, menuAcceptIndex);
                showInfoHeader("Ändere Teilnahme", true)
            }
        }

        MenuItem {
            id: menuItemInterest
            font.family: txtForFontFamily.font
            text: "Interesse/Vorbehalt"
            visible: menuAcceptValue !== 2 ? true : false
            height: visible ? implicitHeight : 0
            onClicked: {
                lDataMeetingInfo.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 2, menuAcceptText, meetingType, menuAcceptIndex);
                showInfoHeader("Ändere Teilnahme", true)
            }
        }

        MenuItem {
            id: menuItemDecline
            font.family: txtForFontFamily.font
            text: "Absagen"
            visible: menuAcceptValue !== 3 ? true : false
            height: visible ? implicitHeight : 0
            onClicked: {
                lDataMeetingInfo.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, 3, menuAcceptText, meetingType, menuAcceptIndex);
                showInfoHeader("Ändere Teilnahme", true)
            }
        }

        MenuItem {
            id: menuItemEdit
            text: "Ändern"
            onClicked: {
                if (menuAcceptValue == 1)
                    showTextDialogAccept("Zusagen", menuAcceptText);
                else if (menuAcceptValue == 2)
                    showTextDialogAccept("Interesse/Vorbehalt", menuAcceptText);
                else if (menuAcceptValue == 3)
                    showTextDialogAccept("Absagen", menuAcceptText);
            }
        }
    }

    Component {
        id: commentView
        MyComponents.CommentItem {

        }
    }

    Component {
        id: acceptView
        MyComponents.AcceptInfoSingleItem {
            onClickedElement: {
                var globalCoordinates = columnLayoutAccepts.mapToItem(flickableCurrentMeetInfo, 0, 0)
                clickedAcceptedUserMenu.y = y + globalCoordinates.y
                menuAcceptIndex = infoIndex
                menuAcceptValue = infoValue
                menuAcceptText = infoTitle
                clickedAcceptedUserMenu.open();
            }
        }
    }

    function showTextDialogAccept(header, text) {
        var component = Qt.createComponent("../components/EditableTextDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
            dialog.headerText = header;
            dialog.parentHeight = mainWindow.height
            dialog.parentWidth =  mainPaneCurrentGame.width
            dialog.textMinSize = 4;
            dialog.editableText = text;
            dialog.acceptedTextEdit.connect(acceptedEditTextDialogAccept);
            dialog.font.family= txtForFontFamily.font
            dialog.open();
        }
    }

    function acceptedEditTextDialogAccept(text) {
        lDataMeetingInfo.startAcceptMeetingInfo(m_gamePlayCurrentItem.index, menuAcceptValue, text, meetingType, menuAcceptIndex);
        showInfoHeader("Ändere Teilnahme", true)
    }

    function toolButtonClicked() {

    }

    function sendNewComment(comment) {
        lDataMeetingInfo.startSendNewComment(m_gamePlayCurrentItem.index, comment, meetingType);
    }

    property var  lDataMeetingInfo;
    property bool isEditMode: false
    property bool isInputAlreadyChanged: false

    function showAllInfoAboutGame() {

        if (meetingType === 0)
            lDataMeetingInfo = gDataMeetingInfo;
        else
            lDataMeetingInfo = gDataTripInfo;
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
            if (meetingType === 0)
                toastManager.show("Info übers Treffen geladen", 2000);
            else
                toastManager.show("Info über die Fahrt geladen", 2000);
            textInfo.text = lDataMeetingInfo.info();
            textWhen.init(lDataMeetingInfo.when())
            textWhere.init(lDataMeetingInfo.where())
            if (textInfo.text.length > 0 && !infoSeperator.userClosedShift)
                infoSeperator.isShiftVisible = true;
            showInfoHeader("", false)
        } else if (result === -5) {
            if (meetingType === 0)
                toastManager.show("Bisher noch kein Treffen gespeichert", 2000);
            else
                toastManager.show("Bisher noch keine Fahrt gespeichert", 2000);
            textInfo.text = "";
            textWhen.init("")
            textWhere.init("")
            showInfoHeader("", false)
        } else {
            showInfoHeader("Infos laden hat nicht funktioniert", false)
        }

//        listViewModelAcceptedUsers.clear();

        if (result === 1 && lDataMeetingInfo.getAcceptedListCount() > 0) {
            if (!acceptSeperator.userClosedShift)
                acceptSeperator.isShiftVisible = true;
            for (var i = 0; i < lDataMeetingInfo.getAcceptedListCount(); i++) {
                var acceptInfo = lDataMeetingInfo.getAcceptInfoFromIndex(i)
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
                var sprAccepts = acceptView.createObject(columnLayoutAccepts)
                sprAccepts.infoTitle = acceptInfo.name();
                sprAccepts.infoIndex = acceptInfo.index();
                sprAccepts.infoValue = acceptInfo.value();
                sprAccepts.imageColor = btnColor;
                sprAccepts.imageSource = imgSource;

//                listViewModelAcceptedUsers.append({
//                                                      title: acceptInfo.name(),
//                                                      index: acceptInfo.index(),
//                                                      color: btnColor,
//                                                      source: imgSource,
//                                                      value: acceptInfo.value()
//                                                  });

            }
            /* Does not work in defintion for freeTickets, so set it here */
//            listViewAcceptedUsers.implicitHeight = listViewModelAcceptedUsers.count * listViewItemHeight

        } else {
//            listViewAcceptedUsers.implicitHeight = 0;
        }

        if (result === 1 && lDataMeetingInfo.getCommentCount() > 0) {
            if (!commentSeperator.userClosedShift)
                commentSeperator.isShiftVisible = true;

            for (var j = 0; j < lDataMeetingInfo.getCommentCount(); j++) {
                var commentItem = lDataMeetingInfo.getCommentFromIndex(j)
                var sprComment = commentView.createObject(columnLayoutComments)
                sprComment.showCommentItem(commentItem);
            }
        }
    }

    function notifyAcceptMeetingFinished(result) {
        if (result === 1) {
            toastManager.show("Teilnahme erfolgreich geändert", 2000);
            loadMeetingInfo();

        } else {
            if (result === -7) {
                var component = Qt.createComponent("../components/AcceptDialog.qml");
                if (component.status === Component.Ready) {
                    var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                    dialog.headerText = "Information";
                    dialog.parentHeight = mainPaneCurrentGame.height
                    dialog.parentWidth = mainPaneCurrentGame.width
                    dialog.textToAccept = "Der Name exisitiert bereits.\n\nZum Ändern eines vorhanden Eintrags auf den Namen klicken.";
                    dialog.showCancelButton = false
                    dialog.font.family= txtForFontFamily.font
                    dialog.open();
                }
            }

            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader("Teilnehmen hat nicht funktioniert", false)
        }
        acceptSeperator.isShiftVisible = true
    }

    function notifySendCommentMeetFinished(result) {
        if (result === 1) {
            toastManager.show("Kommentar erfolgreich gesendet", 2000);
            loadMeetingInfo();

        } else {
            toastManager.show(userIntCurrentGame.getErrorCodeToString(result), 4000);
            showInfoHeader("Kommentar senden hat nicht funktioniert", false)
        }
        commentSeperator.isShiftVisible = true;
    }

    function deleteValues() {

        for (var j = columnLayoutAccepts.children.length; j > 0; j--) {
            columnLayoutAccepts.children[j - 1].destroy()
        }

        for (var j = columnLayoutComments.children.length; j > 0; j--) {
            columnLayoutComments.children[j - 1].destroy()
        }
    }

    function loadMeetingInfo()
    {
//        listViewModelAcceptedUsers.clear();
        lDataMeetingInfo.clearAcceptInfoList();
        lDataMeetingInfo.clearCommentList();
        deleteValues();
        showInfoHeader("Aktualisiere Daten", true)
        //        if (meetingType === 0)
        //            gDataMeetingInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);
        //        else
        //            gDataTripInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);
        lDataMeetingInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);

    }
}





