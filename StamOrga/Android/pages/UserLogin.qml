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

//        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

        ColumnLayout {
            id: mainColumnLayoutUser
            width: pane.width
            spacing: 25

            ColumnLayout {
                id: columnLayoutUserLogin
                width: pane.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                spacing: 25

                TextField {
                    id: txtIPAddress
                    text: globalUserData.ipAddr
                    implicitWidth: mainColumnLayoutUser.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    placeholderText: qsTr("Text Field")
                    visible: userIntUser.isDebuggingEnabled()
                }

    //            SpinBox {
    //                id: spBoxPort
    //                to: 100000
    //                from: 1
    //                value: globalUserData.conMasterPort
    //                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    //            }

                ColumnLayout {
                    spacing: 5
                    width: mainColumnLayoutUser.width
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Label {
                        text: qsTr("Login / Email")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }

                    TextField {
                        id: txtUserName
                        text: globalUserData.userName
                        padding: 8
                        implicitWidth: mainColumnLayoutUser.width / 3 * 2
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onTextChanged: {
                            globalUserData.bIsConnected = false;
                        }
                    }
                }

                ColumnLayout {
                    width: mainColumnLayoutUser.width
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Label {
                        text: qsTr("Passwort")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }

                    TextField {
                        id: txtPassWord
                        text: globalUserData.passWord
                        implicitWidth: mainColumnLayoutUser.width / 3 * 2
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        onTextChanged: {
                            globalUserData.bIsConnected = false;
                        }
                    }
                }

                Button {
                    id: btnSendData
                    text: qsTr("Verbinden")
                    implicitWidth: Math.max(mainColumnLayoutUser.width / 4 * 2, contentWidth)
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        if (userIntUser.isDebuggingEnabled()) {
                            globalUserData.ipAddr = txtIPAddress.text
                        }

                        if (userIntUser.startMainConnection(txtUserName.text, txtPassWord.text) > 0) {
                            btnSendData.enabled = false
                            busyConnectIndicator.visible = true;
                            txtInfoConnecting.text = "Verbinde ..."
                            txtInfoConnecting.visible = true;
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyConnectIndicator
                    opacity: 1
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoConnecting
                    text: qsTr("Label")
                    visible: false
                }
            }

            ColumnLayout {
                id: columnLayoutUserData
                width: pane.width
                spacing: 25
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter


                Button {
                    id: btnChangeReadableName
                    implicitWidth: mainColumnLayoutUser.width / 4 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
//                    enabled: globalUserData.bIsConnected
                    onClicked: {
                        var component = Qt.createComponent("../components/EditableTextDialog.qml");
                        if (component.status === Component.Ready) {
                            var dialog = component.createObject(flickableUser,{popupType: 1});
                            dialog.headerText = "Nutzername ändern";
                            dialog.parentHeight = flickableUser.height
                            dialog.parentWidth = flickableUser.width
                            console.log ("Width = "+ flickableUser.width)
                            dialog.textMinSize = 6
                            dialog.editableText = globalUserData.readableName;
                            dialog.acceptedTextEdit.connect(acceptedEditReadableName);
                            dialog.open();

                            console.log ("X = "+ dialog.x + " ")
                        }

                    }

                    function acceptedEditReadableName(text) {
                        busyConnectIndicator.visible = true;
                        txtInfoConnecting.visible = true;
                        txtInfoConnecting.text = "Ändere Öffentlichen Namen"
                        userIntUser.startUpdateReadableName(text)
                    }
                }

                ToolSeparator {
                    id: toolSeparator3
                    orientation: "Horizontal"
                    implicitWidth: mainColumnLayoutUser.width / 3 * 1
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Button {
                    id: btnChangePassWord
                    text: qsTr("Password ändern")
                    implicitWidth: mainColumnLayoutUser.width / 4 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
//                    enabled: globalUserData.bIsConnected
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
            btnSendData.background.color = "green"
            txtInfoConnecting.text = "Verbindung erfolgreich"
            updateUserColumnView(false);
        }
        else {
            btnSendData.background.color = "red"
            txtInfoConnecting.text = userIntUser.getErrorCodeToString(result);
            updateUserColumnView(true);
        }
    }

    function notifyUserIntUpdatePasswordFinished(result) {
        busyConnectIndicator.visible = false;
        if (result === 1)
            txtInfoConnecting.text = "Passwort erfolgreich geändert"
        else
            txtInfoConnecting.text = "Fehler beim Passwort ändern"
    }


    function notifyUserIntUpdateReadableNameFinished(result) {
        busyConnectIndicator.visible = false;
        if (result === 1)
            txtInfoConnecting.text = "Name erfolgreich geändert"
        else
            txtInfoConnecting.text = "Fehler beim Namen ändern"
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
            if (txtnewPassWord.text.length < 6) {
                labelPasswordTooShort.visible = true
                changePassWordDialog.open()
            } else if (txtnewPassWord.text == txtnewPassWordReplay.text) {
                userIntUser.startUpdateUserPassword(txtnewPassWord.text)
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

                Label {
                    id: labelChaPass1
                    text: qsTr("neues Passwort")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWord
                    text: globalUserData.passWord
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: columnLayoutChPass3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: labelChaPass2
                    text: qsTr("Passwort wiederholen")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWordReplay
                    text: globalUserData.passWord
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            Label {
                id: labelPasswordDiffer
                visible: false
                text: qsTr("Passwörter stimmen nicht überein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "red"
            }
            Label {
                id: labelPasswordTooShort
                visible: false
                text: qsTr("Das Passwort muss mindestens 6 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "orange"
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}



