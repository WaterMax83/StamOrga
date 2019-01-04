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
    id: flickableUserOverview

    contentHeight: mainPaneUserOverview.height

    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableUserOverview.contentY < -refreshItem.refreshHeight) {
            updateUserOverviewList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableUserOverview.contentY
    }

    Pane {
        id: mainPaneUserOverview
        width: parent.width
        padding: 0

        ColumnLayout {
            id: mainuserOverviewColumnLayout
            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorUserOverview
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            Column {
                id: columnLayoutUserOverview
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }
    }

    function toolButtonClicked() {

    }

    function updateUserOverviewList(){
        gDataUserManager.startListUserOverview()
        busyIndicatorUserOverview.loadingVisible = true
        busyIndicatorUserOverview.infoText = "Aktualisiere Benutzer Liste"
        for (var j = columnLayoutUserOverview.children.length; j > 0; j--) {
            columnLayoutUserOverview.children[j-1].destroy()
        }
    }

    function notifyUserCommandFinished(result) {

        busyIndicatorUserOverview.loadingVisible = false
        if (result === 1) {
            toastManager.show("Benutzer geladen", 2000)
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 5000);
        }
        showUserOverview();
    }



    function pageOpenedUpdateView() {

        updateUserOverviewList();
    }

    function showUserOverview() {

        for (var j = columnLayoutUserOverview.children.length; j > 0; j--) {
            columnLayoutUserOverview.children[j-1].destroy()
        }

        if (gDataUserManager.getUserInformationLength() > 0) {
            for (var i=0; i < gDataUserManager.getUserInformationLength(); i++) {
                var sprite = userInformationItem.createObject(columnLayoutUserOverview)
                sprite.showUserInformation(i);
            }
            busyIndicatorUserOverview.infoText = "Letztes Update vor ";// + gDataTicketManager.getSeasonTicketLastLocalUpdateString()
        } else
            busyIndicatorUserOverview.infoText = "Keine Daten gespeichert\nZiehen zum Aktualisieren"
    }

    function notifyUserIntConnectionFinished(result, msg) {}

    Component {
        id: userInformationItem
        MyComponents.UserInformationItem {
//            onClickedSeasonTicket: {
//                seasonTicketClickedMenu.openWithNameAndIndex(sender.name, sender.place, sender.discount, sender.index)
//            }
        }
    }

    Text {
        id: txtForFontFamily
        visible: false
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}
