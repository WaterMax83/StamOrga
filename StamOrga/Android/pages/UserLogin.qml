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
    id: flickableUser
    contentHeight: pane.height

    property UserInterface userIntUser

    Pane {
        id: pane
        width: parent.width

        Column {
            width: parent.width

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorUserlogin
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            ColumnLayout {
                id: columnLayoutUserLogin
                width:  parent.width
                Layout.fillWidth: true
                spacing: 5

                Text {
                    id: txtForFontFamily
                    visible: false
                }

                TextField {
                    id: txtIPAddress
                    font.family: txtForFontFamily.font
                    text: gConUserSettings.getIPAddr()
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    visible: userIntUser.isDebuggingEnabled()
                    Layout.bottomMargin: 35
                }

                Text {
                    text: qsTr("Login / Email")
                    font.pixelSize: 12
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtUserName
                    font.family: txtForFontFamily.font
                    text: gConUserSettings.getUserName()
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
//                        globalUserData.bIsConnected = false;
                        if (text !== gConUserSettings.getUserName()) {
                            txtPassWord.text = "";
                        }
                    }
                    Layout.bottomMargin: 35
                }



                Text {
                    text: qsTr("Passwort")
                    font.pixelSize: 12
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtPassWord
                    font.family: txtForFontFamily.font
                    text: gConUserSettings.getPassWord().length === 0 ? "" : "dEf1AuLt"
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
//                        globalUserData.bIsConnected = false;
                    }
                    echoMode: TextInput.Password
                    Layout.bottomMargin: 35
                }

                MyComponents.CustomButton {
                    id: btnSendData
                    font.family: txtForFontFamily.font
                    text: qsTr("Verbinden")
                    implicitWidth: Math.max(parent.width / 4 * 2, contentWidth)
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
//                        if (userIntUser.isDebuggingEnabled()) {
//                            globalUserData.ipAddr = txtIPAddress.text
//                        }

                        if (txtUserName.text.trim() === "" || txtPassWord.text.trim() === "") {
                            toastManager.show("Bitte Verbindungsdaten ausfüllen", 4000)
                            return;
                        }

                        if (userIntUser.startMainConnection(txtUserName.text.trim(), txtPassWord.text.trim()) > 0) {
                            btnSendData.enabled = false
                            busyIndicatorUserlogin.loadingVisible = true;
                            busyIndicatorUserlogin.infoText = "Verbinde ..."
                            busyIndicatorUserlogin.infoVisible = true;
                        }
                    }
                }
            }

            ColumnLayout {
                id: columnLayoutUserData
                width: parent.width
                spacing: 5

                  MyComponents.CustomButton {
                    id: btnChangeReadableName
                    font.family: txtForFontFamily.font
                    implicitWidth: parent.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        var component = Qt.createComponent("../components/EditableTextDialog.qml");
                        if (component.status === Component.Ready) {
                            var dialog = component.createObject(flickableUser,{popupType: 1});
                            if (gConUserSettings.getReadableName() === "")
                                dialog.headerText = "Nutzername anlegen";
                            else
                                dialog.headerText = "Nutzername ändern";
                            dialog.font.family= txtForFontFamily.font
                            dialog.parentHeight = flickableUser.height
                            dialog.parentWidth = flickableUser.width
                            dialog.textMinSize = 4;
                            dialog.editableText = gConUserSettings.getReadableName();
                            dialog.acceptedTextEdit.connect(acceptedEditReadableName);
                            dialog.open();
                        }

                    }

                    function acceptedEditReadableName(text) {
                        busyIndicatorUserlogin.loadingVisible = true;
                        busyIndicatorUserlogin.infoVisible = true;
                        busyIndicatorUserlogin.infoText = "Ändere Nutzernamen"
                        gConUserSettings.startUpdateReadableName(text.trim());
//                        userIntUser.startUpdateReadableName(text)
                    }
                }
                Text {
                    id: txtInfoReadableName
    //                width: parent.width / 3 * 2
                    text: qsTr("Der Nutzername dient als Anzeige für andere Nutzer und zur Vorauswahl für editierbare Textfelder")
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                    Layout.maximumWidth: parent.width - 20
                    visible: true
                    Layout.bottomMargin: 35
                    font.pixelSize: 12
                    color: "white"
                }

                ToolSeparator {
                    id: toolSeparator3
                    orientation: "Horizontal"
                    implicitWidth: parent.width / 3 * 1
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.bottomMargin: 35
                }

                MyComponents.CustomButton {
                    id: btnChangePassWord
                    font.family: txtForFontFamily.font
                    text: qsTr("Passwort ändern")
                    implicitWidth: parent.width / 4 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        txtnewPassWord.text = ""
                        txtnewPassWordReplay.text = ""
                        changePassWordDialog.open()
                    }
                }

                ToolSeparator {
                    id: toolSeparator4
                    orientation: "Horizontal"
                    implicitWidth: parent.width / 3 * 1
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.bottomMargin: 30
                    Layout.topMargin: 30
                }

                MyComponents.CustomButton {
                    id: btnBacktoStart
                    font.family: txtForFontFamily.font
                    text: qsTr("< Zurück zur StartSeite")
                    implicitWidth: parent.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: stackView.pop()
                }

            }
        }
    }

    function toolButtonClicked() {
        optionsMenuUserLogin.open();
    }

    Menu {
        id: optionsMenuUserLogin
        x: parent.width - width
        transformOrigin: Menu.TopRight

        MenuItem {
            text: "UserLogin anzeigen"
            onTriggered: updateUserColumnView(true);
        }
    }


    function notifyUserIntConnectionFinished(result, msg) {
        btnSendData.enabled = true
        busyIndicatorUserlogin.loadingVisible = false;
        if (result === 1) {
            busyIndicatorUserlogin.infoText = "Verbindung erfolgreich";
            toastManager.show("Verbindung erfolgreich", 2000)
            updateUserColumnView(false);
        }
        else {
            if (result === -21)
                txtPassWord.text = "";
            busyIndicatorUserlogin.infoVisible = false;
            busyIndicatorUserlogin.infoText = userIntUser.getErrorCodeToString(result);
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
            updateUserColumnView(true);
        }
    }

    function notifyUserIntUpdatePasswordFinished(result) {
        busyIndicatorUserlogin.loadingVisible = false;
        busyIndicatorUserlogin.infoVisible = false;
        if (result === 1) {
            toastManager.show("Passwort erfolgreich geändert", 2000)
            busyIndicatorUserlogin.infoVisible = false;
        }
        else {
            busyIndicatorUserlogin.infoText = "Fehler beim Passwort ändern"
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        }
    }


    function notifyUserIntUpdateReadableNameFinished(result) {
        busyIndicatorUserlogin.loadingVisible = false;

        if (result === 1) {
            toastManager.show("Nutzername erfolgreich geändert", 2000)
            busyIndicatorUserlogin.infoVisible = false;
        }
        else {
            busyIndicatorUserlogin.infoText = "Fehler beim Namen ändern"
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        }
    }

    function pageOpenedUpdateView() {
        if (gConUserSettings.getUserName() === "" || gConUserSettings.getPassWord() === "")
            updateUserColumnView(true);
        else
            updateUserColumnView(false);
    }

    function updateUserColumnView(enableLogin) {
        if (enableLogin) {
            columnLayoutUserLogin.visible = true;
            columnLayoutUserData.visible = false;
        } else {
            columnLayoutUserLogin.visible = false;
            columnLayoutUserData.visible = true;
            if (gConUserSettings.getReadableName() === "")
                btnChangeReadableName.text = "Nutzername anlegen"
            else
                btnChangeReadableName.text = "Nutzername ändern"
        }
    }

    MyComponents.CustomDialog {
        id: changePassWordDialog
        font.family: txtForFontFamily.font
        x: Math.round((flickableUser.width - width) / 2)
        y: Math.round(flickableUser.height / 6)
        width: Math.round(Math.min(flickableUser.width, flickableUser.height) / 3 * 2)
        modal: true
        focus: true
        title: "Password ändern"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            labelPasswordTooShort.visible = false
            labelPasswordDiffer.visible = false
            if (txtnewPassWord.text.trim().length < 6) {
                labelPasswordTooShort.visible = true
                changePassWordDialog.open()
            } else if (txtnewPassWord.text.trim() == txtnewPassWordReplay.text.trim()) {
                gConUserSettings.startUpdatePassword(txtnewPassWord.text.trim());
//                userIntUser.startUpdateUserPassword()
                busyIndicatorUserlogin.loadingVisible = true;
                busyIndicatorUserlogin.infoVisible = true;
                busyIndicatorUserlogin.infoText = "Ändere Passwort"
                changePassWordDialog.close()
            } else {
                labelPasswordDiffer.visible = true
                changePassWordDialog.open()
            }
        }
        onRejected: {
            changePassWordDialog.close()
            labelPasswordTooShort.visible = false
            labelPasswordDiffer.visible = false
        }

        contentItem: ColumnLayout {
            id: changePasswordColumn
            width: changePassWordDialog.width
            spacing: 20

            ColumnLayout {
                id: columnLayoutChPass2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Text {
                    id: labelChaPass1
                    font.pixelSize: 12
                    color: "white"
                    text: qsTr("neues Passwort")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWord
                    font.family: txtForFontFamily.font
                    text: gConUserSettings.getPassWord()
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    echoMode: TextInput.Password
                }
            }

            ColumnLayout {
                id: columnLayoutChPass3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Text {
                    id: labelChaPass2
                    font.pixelSize: 12
                    color: "white"
                    text: qsTr("Passwort wiederholen")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWordReplay
                    font.family: txtForFontFamily.font
                    text: gConUserSettings.getPassWord()
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    echoMode: TextInput.Password
                }
            }

            Text {
                id: labelPasswordDiffer
                visible: false
                text: qsTr("Passwörter stimmen nicht überein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                font.pixelSize: 12
                color: "red"
            }
            Text {
                id: labelPasswordTooShort
                visible: false
                text: qsTr("Das Passwort muss mindestens 6 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                font.pixelSize: 12
                color: "orange"
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}



