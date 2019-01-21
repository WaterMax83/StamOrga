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
import QtQuick.Controls.Material 2.2

import "../components" as MyComponents
import "../controls" as MyControls

Flickable {
    id: flickableUserProfil
    contentHeight: mainPaneUserProfil.height

    boundsBehavior: Flickable.StopAtBounds

    Pane {
        id: mainPaneUserProfil
        width: parent.width

        Column {
            width: parent.width

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorUserProfil
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            ColumnLayout {
                width: parent.width
                spacing: 5

                Text {
                    id: txtForFontFamily
                    visible: false
                }


                Item {
                    width: parent.width
                    height: column1.implicitHeight

                    ColumnLayout {
                        id: column1
                        width: parent.width
                        spacing: 5

                        Text {
                            id: text1
                            text: "Nutzername"
                            color: "white"
                            font.pixelSize: 16
                            Layout.fillWidth: true
                        }

                        Text {
                            id: textPreviewUserName
                            text: gConUserSettings.getReadableName()
                            color: Material.hintTextColor
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                            Layout.bottomMargin: 5
                        }
                    }
                    MouseArea {
                        id: mouse1
                        anchors.fill: parent
                        onClicked: {
                            var component = Qt.createComponent("../components/EditableTextDialog.qml");
                            if (component.status === Component.Ready) {
                                var dialog = component.createObject(flickableUserProfil,{popupType: 1});
                                if (gConUserSettings.getReadableName() === "")
                                    dialog.headerText = "Nutzername anlegen";
                                else
                                    dialog.headerText = "Nutzername ändern";
                                dialog.font.family= txtForFontFamily.font
                                dialog.parentHeight = flickableUserProfil.height
                                dialog.parentWidth = flickableUserProfil.width
                                dialog.textMinSize = 4;
                                dialog.editableText = gConUserSettings.getReadableName();
                                dialog.acceptedTextEdit.connect(acceptedEditReadableName);
                                dialog.open();
                            }
                        }

                        function acceptedEditReadableName(text) {
                            busyIndicatorUserProfil.loadingVisible = true;
                            busyIndicatorUserProfil.infoVisible = true;
                            busyIndicatorUserProfil.infoText = "Ändere Nutzernamen"

                            gDataUserManager.startUpdateReadableName(text.trim());
                        }
                    }
                }

                Rectangle {
                    color: Material.hintTextColor
                    height: 1
                    Layout.fillWidth: true
                }

                Item {
                    id: itemEmailNotify
                    width: parent.width
                    height: column2.height
                    visible: gConUserSettings.getEmailNotification() !== -2

                    ColumnLayout {
                        id: column2
                        width: parent.width

                        Text {
                            text: "Email Benachrichtigung"
                            color: "white"
                            font.pixelSize: 16
                            Layout.fillWidth: true
                        }

                        Text {
                            id: emailNotifyText
                            text: gConUserSettings.getEmailNotifyString()
                            color: Material.hintTextColor
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (gConUserSettings.getEmailNotification() !== -2) {
                                if (gDataUserManager.startChangeEmailNotification() === 1) {
                                    busyIndicatorUserProfil.loadingVisible = true;
                                    busyIndicatorUserProfil.infoVisible = true;
                                    busyIndicatorUserProfil.infoText = "Ändere Aktivierung"
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    color: Material.hintTextColor
                    height: 1
                    Layout.fillWidth: true
                    visible: gConUserSettings.getEmailNotification() !== -2
                }

                Item {
                    width: parent.width
                    height: column3.height

                    ColumnLayout {
                        id: column3
                        width: parent.width

                        Text {
                            text: "Passwort ändern"
                            color: "white"
                            font.pixelSize: 16
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "********"
                            color: Material.hintTextColor
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            txtnewPassWord.text = ""
                            txtnewPassWordReplay.text = ""
                            changePassWordDialog.open()
                        }
                    }
                }

                Rectangle {
                    color: Material.hintTextColor
                    height: 1
                    Layout.fillWidth: true
                }

                Item {
                    width: parent.width
                    height: column4.height

                    ColumnLayout {
                        id: column4
                        width: parent.width

                        Text {
                            text: "Benutzer wechseln"
                            color: "white"
                            font.pixelSize: 16
                            Layout.fillWidth: true
                        }

                        Text {
                            text: gConUserSettings.getUserName();
                            color: Material.hintTextColor
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            stackView.push(viewUserLogin)
                        }
                    }
                }

                Rectangle {
                    color: Material.hintTextColor
                    height: 1
                    Layout.fillWidth: true
                }

//                Text {
//                    id: txtInfoReadableName
//                    //                width: parent.width / 3 * 2
//                    text: qsTr("Der Nutzername dient als Anzeige für andere Nutzer und zur Vorauswahl für editierbare Textfelder")
//                    horizontalAlignment: Text.AlignHCenter
//                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
//                    wrapMode: Text.WordWrap
//                    Layout.maximumWidth: parent.width - 20
//                    visible: true
//                    Layout.bottomMargin: 35
//                    font.pixelSize: 12
//                    color: "white"
//                }
            }
        }
    }

    function toolButtonClicked() { }

    //    Menu {
    //        id: optionsMenuUserLogin
    //        x: parent.width - width
    //        transformOrigin: Menu.TopRight

    //        MenuItem {
    //            text: "UserLogin anzeigen"
    //            onTriggered: updateUserColumnView(true);
    //        }
    //    }

    function notifyUserIntConnectionFinished(result) { }

    function notifyUserCommandFinished(result, subCmd) {
        busyIndicatorUserProfil.loadingVisible = false;

        if (subCmd === 1) { // change readableName
            if (result === 1) {
                toastManager.show("Nutzername erfolgreich geändert", 2000)
                busyIndicatorUserProfil.infoVisible = false;
                textPreviewUserName.text = gConUserSettings.getReadableName();
            }
            else {
                busyIndicatorUserProfil.infoText = "Fehler beim Namen Ändern"
                toastManager.show(userInt.getErrorCodeToString(result), 5000)
            }
        } else if (subCmd === 2) { // change password
            if (result === 1) {
                toastManager.show("Passwort erfolgreich geändert", 2000)
                busyIndicatorUserProfil.infoVisible = false;
            }
            else {
                busyIndicatorUserProfil.infoText = "Fehler beim Passwort Ändern"
                toastManager.show(userInt.getErrorCodeToString(result), 5000)
            }
        } else if (subCmd === 3)   { // change email notify
            if (result === 1) {
                toastManager.show("Aktivierung erfolgreich geändert", 2000)
                busyIndicatorUserProfil.infoVisible = false;
                emailNotifyText.text = gConUserSettings.getEmailNotifyString();
            } else {
                busyIndicatorUserProfil.infoText = "Fehler beim Ändern der Aktivierung"
                toastManager.show(userInt.getErrorCodeToString(result), 5000)
            }
        }
    }

    function pageOpenedUpdateView() {

        if (gConUserSettings.getEmailNotification() === -1) {
            busyIndicatorUserProfil.loadingVisible = true;
            busyIndicatorUserProfil.infoVisible = true;
            busyIndicatorUserProfil.infoText = "Lade Daten"
            gConUserSettings.startGettingUserProps(true);
        }

    }

    function notifyGetUserProperties(result) {
        busyIndicatorUserProfil.loadingVisible = false;

        if (result === 1) {
            busyIndicatorUserProfil.infoVisible = false;
            emailNotifyText.text = gConUserSettings.getEmailNotifyString();
        } else {
            busyIndicatorUserProfil.infoText = "Fehler beim Laden der Daten"
            toastManager.show(userInt.getErrorCodeToString(result), 5000)
        }
    }

    MyComponents.CustomDialog {
        id: changePassWordDialog
        font.family: txtForFontFamily.font
        x: Math.round((flickableUserProfil.width - width) / 2)
        y: Math.round(flickableUserProfil.height / 6)
        width: Math.round(Math.min(flickableUserProfil.width, flickableUserProfil.height) / 3 * 2)
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

                gDataUserManager.startUpdatePassword(txtnewPassWord.text.trim());

                busyIndicatorUserProfil.loadingVisible = true;
                busyIndicatorUserProfil.infoVisible = true;
                busyIndicatorUserProfil.infoText = "Ändere Passwort"
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



