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
    id: flickableFanclubNewsList
    flickableDirection: Flickable.VerticalFlick

    contentHeight: mainPaneFanClubNewsList.height

    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableFanclubNewsList.contentY < -refreshItem.refreshHeight) {
            updateFanclubNewsList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableFanclubNewsList.contentY
    }

    Pane {
        id: mainPaneFanClubNewsList
        width: parent.width
//        height: parent.height
        padding: 0

        ColumnLayout {
            id: mainFanClubColumnLayout
            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorNewsList
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            Column {
                id: columnLayoutFanClubList
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }

    }

    property bool isViewAlreadyOpened: false
    function pageOpenedUpdateView() {

        if (gConUserSettings.userIsFanclubEditEnabled() ||  userInt.isDebuggingEnabled())
            updateHeaderFromMain("Fanclub", "images/add.png")
        else
            updateHeaderFromMain("Fanclub", "")

        if (!isViewAlreadyOpened)
            updateFanclubNewsList();
        else
            showNewsDataList();
        isViewAlreadyOpened = true;
    }

    function toolButtonClicked() {
        if (!gConUserSettings.userIsFanclubEditEnabled() &&  !userInt.isDebuggingEnabled())
            return;

        var component = Qt.createComponent("../pages/FanclubNewsItem.qml")
        if (component.status === Component.Ready) {
            var sprite = stackView.push(component)
            sprite.userIntCurrentNews = userInt
            sprite.startShowElements(undefined, true);
        }
    }

    function updateFanclubNewsList() {
        gDataNewsDataManager.startListNewsData();
        busyIndicatorNewsList.loadingVisible = true;
        busyIndicatorNewsList.infoText = "Aktualisiere Liste"
    }

    function notifyFanclubNewsListFinished(result){
        if (result === 1) {
            if (gDataAppUserEvents.getCurrentFanclubEventCounter() > 0) {
                gConUserSettings.startGettingUserProps(true);
                return;
            }
            toastManager.show("Liste erfolgreich geladen", 2000);
            busyIndicatorNewsList.loadingVisible = false;
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorNewsList.loadingVisible = false;
            busyIndicatorNewsList.infoText = "Liste konnte nicht geladen werden"
        }
        showNewsDataList();
    }

    function notifyDeleteFanclubNewsItemFinished(result) {
        if (result === 1) {
            toastManager.show("Nachricht erfolgreich gelöscht", 2000);
            updateFanclubNewsList();
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorNewsList.loadingVisible = false;
            busyIndicatorNewsList.infoText = "Nachricht konnte nicht gelöscht werden"
        }
    }

    function notifyGetUserProperties(result) {
        if (result === 1) {
            toastManager.show("Liste erfolgreich geladen", 2000);
            busyIndicatorNewsList.loadingVisible = false;
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorNewsList.loadingVisible = false;
            busyIndicatorNewsList.infoText = "Fehler beim Laden der Events"
        }

        showNewsDataList();
    }

    function showNewsDataList() {

        for (var j = columnLayoutFanClubList.children.length; j > 0; j--) {
            columnLayoutFanClubList.children[j-1].destroy()
        }

        if (gDataNewsDataManager.getNewsDataLength() > 0) {
            for (var i = 0; i < gDataNewsDataManager.getNewsDataLength(); i++) {
                var sprite = newsDataItem.createObject(columnLayoutFanClubList)
                sprite.showTextDataInfo(i, 0);
            }
            busyIndicatorNewsList.infoText = "Letztes Update vor " + gDataNewsDataManager.getNewsDataLastLocalUpdateString()
        } else
            busyIndicatorNewsList.infoText = "Keine Daten gespeichert\nZiehen zum Aktualisieren"

    }

    Component {
        id: newsDataItem
        MyComponents.TextDataDesignItem {
            onClickedItem: {
                var component = Qt.createComponent("../pages/FanclubNewsItem.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    sprite.userIntCurrentNews = userInt
                    sprite.startShowElements(sender, false);

                    if (sender.event)
                        gDataAppUserEvents.clearUserEventFanclub(sender.index);
                }
            }
            onPressAndHoldItem: {
                if (!gConUserSettings.userIsFanclubEditEnabled() &&  !userInt.isDebuggingEnabled())
                    return;

                m_newsItemEditIndex = sender.index;

                newsItemClickedMenu.open();
            }
        }
    }

    property var m_newsItemEditIndex;

    Menu {
            id: newsItemClickedMenu
            x: (flickableFanclubNewsList.width - width) / 2
            y: flickableFanclubNewsList.height / 6

            background: Rectangle {
                    implicitWidth: menuItemDelete.width
                    color: "#4f4f4f"
                }

            MenuItem {
                id: menuItemDelete
                text: "Löschen"
                onClicked: {
                    var component = Qt.createComponent("../components/AcceptDialog.qml");
                    if (component.status === Component.Ready) {
                        var dialog = component.createObject(flickableFanclubNewsList,{popupType: 1});
                        dialog.headerText = "Bestätigung";
                        dialog.parentHeight = flickableFanclubNewsList.height
                        dialog.parentWidth = flickableFanclubNewsList.width
                        dialog.textToAccept = "Soll die Nachricht wirklich gelöscht werden?";
                        dialog.acceptedDialog.connect(acceptedDeletingNews);
                        dialog.open();
                    }
                }
            }
    }

    function acceptedDeletingNews() {
        busyIndicatorNewsList.loadingVisible = true;
        busyIndicatorNewsList.infoText = "Lösche Nachricht"
        gDataNewsDataManager.startRemoveNewsDataItem(m_newsItemEditIndex);
//        userInt.startDeleteFanclubNewsItem(m_newsItemEditIndex);
    }

    function notifyUserIntConnectionFinished(result) {}
}
