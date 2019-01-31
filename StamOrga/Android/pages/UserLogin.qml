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
import "../controls" as MyControls

Flickable {
    id: flickableUserLogin
    contentHeight: mainPaneUserLogin.height

    boundsBehavior: Flickable.StopAtBounds

    Pane {
        id: mainPaneUserLogin
        width: parent.width

        Column {
            width: parent.width

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorUserlogin
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                Layout.bottomMargin: 10
                infoVisible: false
            }

            ColumnLayout {
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
                    implicitWidth: parent.width / 5 * 4
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    visible: userInt.isDebuggingEnabled()
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
                    implicitWidth: parent.width / 5 * 4
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
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
                    implicitWidth: parent.width / 5 * 4
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onTextChanged: {
                        //                        globalUserData.bIsConnected = false;
                    }
                    echoMode: TextInput.Password
                    Layout.bottomMargin: 35
                }

                MyControls.Button {
                    id: btnLogin
                    font.family: txtForFontFamily.font
                    text: qsTr("Verbinden")
                    enabled: !bDisableLoginButton && txtUserName.text.length > 3 && txtPassWord.text.length > 3
                    implicitWidth: Math.max(parent.width / 4 * 2, contentWidth)
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    transformOrigin: Item.Center
                    onClicked: {
                        if (txtUserName.text.trim() === "" || txtPassWord.text.trim() === "") {
                            toastManager.show("Bitte Verbindungsdaten ausfüllen", 4000)
                            return;
                        }

                        if (userInt.startMainConnection(txtUserName.text.trim(), txtPassWord.text.trim()) > 0) {
                            bDisableLoginButton = true
                            busyIndicatorUserlogin.loadingVisible = true;
                            busyIndicatorUserlogin.infoText = "Verbinde ..."
                            busyIndicatorUserlogin.infoVisible = true;
                        }
                    }
                }
            }
        }
    }

    property bool bDisableLoginButton : false

    function toolButtonClicked() { }

    function notifyUserIntConnectionFinished(result) {
        bDisableLoginButton = false;
        busyIndicatorUserlogin.loadingVisible = false;
        if (result === 1) {
            //            busyIndicatorUserlogin.infoText = "Verbindung erfolgreich";
            busyIndicatorUserlogin.infoVisible = false;
            toastManager.show("Verbindung erfolgreich", 2000)
            while(stackView.depth > 1)
                stackView.pop();
//            stackView.initialItem
//            stackView.clear();
//            stackView.push(viewMainGames);
//            stackView.push(viewUserProfil);
        }
        else {
            if (result === -21)
                txtPassWord.text = "";
            busyIndicatorUserlogin.infoVisible = true;
            busyIndicatorUserlogin.infoText = userInt.getErrorCodeToString(result);
            toastManager.show(userInt.getErrorCodeToString(result), 5000)
        }
    }

    function pageOpenedUpdateView() {
        txtUserName.text = gConUserSettings.getUserName()
        txtPassWord.text = gConUserSettings.getPassWord().length === 0 ? "" : "dEf1AuLt"
    }

    function notifyUserCommandFinished(result, subCmd) { }

    ScrollIndicator.vertical: ScrollIndicator { }
}



