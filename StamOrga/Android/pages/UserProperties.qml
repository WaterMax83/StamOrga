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
    id: flickableUser
    contentHeight: pane.height

    boundsBehavior: Flickable.StopAtBounds

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

                Text {
                    id: textPreviewUserName
//                    text: gConUserSettings.getReadableName()
                    color: "white"
                    font.pixelSize: 16
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
//                    Layout.bottomMargin: 5
                }

                Text {
                    id: textPreviewReadableName
//                    text: gConUserSettings.getReadableName()
                    color: "white"
                    font.pixelSize: 16
                    Layout.fillWidth: true
                    Layout.leftMargin: 10
//                    Layout.bottomMargin: 5
                }





            }
        }
    }

    function toolButtonClicked() {

    }

    function notifyUserIntConnectionFinished(result) { }


    function notifyUserCommandFinished(result, subCmd) {
        //        busyIndicatorUserlogin.loadingVisible = false;
        //        busyIndicatorUserlogin.infoVisible = false;

        //        if (subCmd === 1) { // change readableName
        //            if (result === 1) {
        //                toastManager.show("Nutzername erfolgreich geändert", 2000)
        //                busyIndicatorUserlogin.infoVisible = false;
        //            }
        //            else {
        //                busyIndicatorUserlogin.infoText = "Fehler beim Namen ändern"
        //                toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        //            }
        //        } else if (subCmd === 2) { // change password
        //            if (result === 1) {
        //                toastManager.show("Passwort erfolgreich geändert", 2000)
        //                busyIndicatorUserlogin.infoVisible = false;
        //            }
        //            else {
        //                busyIndicatorUserlogin.infoText = "Fehler beim Passwort ändern"
        //                toastManager.show(userIntUser.getErrorCodeToString(result), 5000)
        //            }
        //        }
    }

    function pageOpenedUpdateView() {
        updateHeaderFromMain("Berechtigungen", "")
    }

    function updatePageWithUser(sender) {
        textPreviewUserName.text = sender.user
        textPreviewReadableName.text = sender.readName
    }



    ScrollIndicator.vertical: ScrollIndicator { }
}



