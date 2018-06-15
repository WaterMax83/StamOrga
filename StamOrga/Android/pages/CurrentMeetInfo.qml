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

    onContentHeightChanged: {
        if (isLastLoadFromComment)
            flickableCurrentMeetInfo.contentY = flickableCurrentMeetInfo.contentHeight - flickableCurrentMeetInfo.height
        isLastLoadFromComment = false;
    }

    onDragEnded: {
        if (flickableCurrentMeetInfo.contentY < -refreshItem.refreshHeight) {
            lDataMeetingInfo.clearCommentList();
            deleteValues();
            showInfoHeader("Aktualisiere Daten", true)
            lDataMeetingInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableCurrentMeetInfo.contentY
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
                Layout.fillWidth: true

                MyComponents.GraphicalButton {
                    imageSource: "../images/edit.png"
                    enabled: isEditMode ? false : true
                    onClickedButton: {
                        isEditMode = true;
                        infoSeperator.visible = true;
                        infoSeperator.userClosedShift = false;
                        infoSeperator.isShiftVisible = true;
                    }
                }

                Item { Layout.fillWidth: true }

                MyComponents.GraphicalButton {
                    imageSource: "../images/save.png"
                    enabled: isEditMode ? true : false
                    onClickedButton: {
                        isEditMode = false;
                        var result = lDataMeetingInfo.startSaveMeetingInfo(m_gamePlayCurrentItem.index, textWhen.input, textWhere.input,
                                                                           textInfo.input, meetingType);
                        if (result === 0)
                            toastManager.show("Keine Änderung, nichts gespeichert", 2000);
                        else {
                            showInfoHeader("Speichere Infos", true)
                        }

                    }
                }

                Item { Layout.fillWidth: true }

                Text {
                    id: fillText
                    text: " - "
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Item { Layout.fillWidth: true }

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
                }

                Item { Layout.fillWidth: true }

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
                }

                Item { Layout.fillWidth: true }

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
                }
            }

            MyComponents.EditableTextWithHint {
                id: textWhere
                hint: "Wo"
                imageSource: meetingType === 0 ? "../images/place.png" : "../images/train.png"
                enableImage: true
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#536878";

                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.EditableTextWithHint {
                id: textWhen
                hint: "Wann"
                imageSource: "../images/time.png"
                enableImage: true
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#536878";
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.ShiftableSeperator {
                id: infoSeperator
                width: parent.width
                textToShow: "Info"
                visible: false
            }

            MyComponents.EditableTextWithHint {
                id: textInfo
                hint: "Information"
                enableImage: false
                width: parent.width
                enabled: isEditMode
                visible: infoSeperator.isShiftVisible
                color: isEditMode ? "#FFFFFF" : "#536878";
                onTextInputChanged: checkNewTextInput()
            }

            MyComponents.ShiftableSeperator {
                id: acceptSeperator
                width: parent.width
                textToShow: "Zusagen"
                visible: false
            }

            Column {
                id: columnLayoutAccepts
                anchors.right: parent.right
                anchors.left: parent.left
                spacing: 5
                visible: acceptSeperator.isShiftVisible
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            MyComponents.ShiftableSeperator {
                id: commentSeperator
                width: parent.width
                textToShow: "Kommentare"
                visible: false
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
    property bool isLastLoadFromComment : false

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
            textInfo.init(lDataMeetingInfo.info());
            textWhen.init(lDataMeetingInfo.when());
            textWhere.init(lDataMeetingInfo.where());

            if (textInfo.input.length > 0) {
                infoSeperator.visible = true;
                if (!infoSeperator.userClosedShift)
                    infoSeperator.isShiftVisible = true;
            }
            showInfoHeader("", false)
        } else if (result === -5) {
            if (meetingType === 0)
                toastManager.show("Bisher noch kein Treffen gespeichert", 2000);
            else
                toastManager.show("Bisher noch keine Fahrt gespeichert", 2000);
            textInfo.init("");
            textWhen.init("")
            textWhere.init("")
            showInfoHeader("", false)
        } else {
            showInfoHeader("Infos laden hat nicht funktioniert", false)
        }

        if (result === 1 && lDataMeetingInfo.getAcceptedListCount() > 0) {
            acceptSeperator.visible = true;
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
            }
        }

        if (result === 1 && lDataMeetingInfo.getCommentCount() > 0) {
            commentSeperator.visible = true;
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
            isLastLoadFromComment = true;
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
        lDataMeetingInfo.clearAcceptInfoList();
        lDataMeetingInfo.clearCommentList();
        deleteValues();
        showInfoHeader("Aktualisiere Daten", true)
        lDataMeetingInfo.startLoadMeetingInfo(m_gamePlayCurrentItem.index, meetingType);

    }
}





