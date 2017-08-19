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

//import "../components" as MyComponents

Flickable {
    id: flickableUser
    contentHeight: pane.height

    property UserInterface userIntUser

    Pane {
        id: pane
        width: parent.width

        Column {
            width: parent.width

            ColumnLayout {
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyConnectIndicator
                    opacity: 1
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Text {
                    id: txtInfoConnecting
                    visible: false
                    font.pixelSize: 10
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.bottomMargin: 35
                }
            }

            ColumnLayout {
                id: columnLayoutUserLogin
                width:  parent.width
                Layout.fillWidth: true
                spacing: 5

                TextField {
                    id: txtIPAddress
                    text: globalUserData.ipAddr
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    visible: userIntUser.isDebuggingEnabled()
                    Layout.bottomMargin: 35
                }

    //            SpinBox {
    //                id: spBoxPort
    //                to: 100000
    //                from: 1
    //                value: globalUserData.conMasterPort
    //                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    //            }


                Text {
                    text: qsTr("Login / Email")
                    font.pixelSize: 12
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtUserName
                    text: globalUserData.userName
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
                        globalUserData.bIsConnected = false;
                        if (text != globalUserData.userName) {
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
                    text: globalUserData.passWord.length === 0 ? "" : "dEf1AuLt"
                    padding: 10
                    implicitWidth: parent.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
                        globalUserData.bIsConnected = false;
                    }
                    echoMode: TextInput.Password
                    Layout.bottomMargin: 35
                }

                Button {
                    id: btnSendData
                    text: qsTr("Verbinden")
                    implicitWidth: Math.max(parent.width / 4 * 2, contentWidth)
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        if (userIntUser.isDebuggingEnabled()) {
                            globalUserData.ipAddr = txtIPAddress.text
                        }

                        if (txtUserName.text.trim() === "" || txtPassWord.text.trim() === "") {
                            toastManager.show("Bitte Verbindungsdaten ausfüllen", 4000)
                            return;
                        }

                        if (userIntUser.startMainConnection(txtUserName.text.trim(), txtPassWord.text.trim()) > 0) {
                            btnSendData.enabled = false
                            busyConnectIndicator.visible = true;
                            txtInfoConnecting.text = "Verbinde ..."
                            txtInfoConnecting.visible = true;
                        }
                    }
                }
            }

            ColumnLayout {
                id: columnLayoutUserData
                width: parent.width
                spacing: 5

                Button {
                    id: btnChangeReadableName
                    implicitWidth: parent.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        var component = Qt.createComponent("../components/EditableTextDialog.qml");
                        if (component.status === Component.Ready) {
                            var dialog = component.createObject(flickableUser,{popupType: 1});
                            if (globalUserData.readableName === "")
                                dialog.headerText = "Nutzername anlegen";
                            else
                                dialog.headerText = "Nutzername ändern";
                            dialog.parentHeight = flickableUser.height
                            dialog.parentWidth = flickableUser.width
                            dialog.textMinSize = 4;
                            dialog.editableText = globalUserData.readableName;
                            dialog.acceptedTextEdit.connect(acceptedEditReadableName);
                            dialog.open();
                        }

                    }

                    function acceptedEditReadableName(text) {
                        busyConnectIndicator.visible = true;
                        txtInfoConnecting.visible = true;
                        txtInfoConnecting.text = "Ändere Nutzernamen"
                        userIntUser.startUpdateReadableName(text)
                    }
                }
                Text {
                    id: txtInfoReadableName
    //                width: parent.width / 3 * 2
                    text: qsTr("Der Nutzername dient als Vorauswahl für alle editierbaren Textfelder")
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

                Button {
                    id: btnChangePassWord
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


    function notifyUserIntConnectionFinished(result) {
        btnSendData.enabled = true
        busyConnectIndicator.visible = false;
        if (result === 1) {
            txtInfoConnecting.text = "Verbindung erfolgreich";
            toastManager.show("Verbindung erfolgreich", 2000)
            updateUserColumnView(false);
        }
        else {
            if (result === -21)
                txtPassWord.text = "";
            txtInfoConnecting.visible = false;
            txtInfoConnecting.text = userIntUser.getErrorCodeToString(result);
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
            updateUserColumnView(true);
        }
    }

    function notifyUserIntUpdatePasswordFinished(result) {
        busyConnectIndicator.visible = false;
        txtInfoConnecting.visible = false;
        if (result === 1) {
            toastManager.show("Passwort erfolgreich geändert", 2000)
            txtInfoConnecting.visible = false;
        }
        else {
            txtInfoConnecting.text = "Fehler beim Passwort ändern"
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        }
    }


    function notifyUserIntUpdateReadableNameFinished(result) {
        busyConnectIndicator.visible = false;

        if (result === 1) {
            toastManager.show("Nutzername erfolgreich geändert", 2000)
            txtInfoConnecting.visible = false;
        }
        else {
            txtInfoConnecting.text = "Fehler beim Namen ändern"
            toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        }
    }

    function pageOpenedUpdateView() {
        if (globalUserData.userName === "" || globalUserData.passWord === "")
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
            if (globalUserData.readableName === "")
                btnChangeReadableName.text = "Nutzername anlegen"
            else
                btnChangeReadableName.text = "Nutzername ändern"
        }
    }

    Dialog {
        id: changePassWordDialog
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
                userIntUser.startUpdateUserPassword(txtnewPassWord.text.trim())
                busyConnectIndicator.visible = true;
                txtInfoConnecting.visible = true;
                txtInfoConnecting.text = "Ändere Passwort"
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
                    text: globalUserData.passWord
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
                    text: globalUserData.passWord
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
                font.pixelSize: 10
                color: "red"
            }
            Text {
                id: labelPasswordTooShort
                visible: false
                text: qsTr("Das Passwort muss mindestens 6 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                font.pixelSize: 10
                color: "orange"
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}



