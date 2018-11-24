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
    id: flickableWebList
    flickableDirection: Flickable.VerticalFlick

    contentHeight: mainPaneWebList.height

    rebound: Transition {
            NumberAnimation {
                properties: "y"
                duration: 1000
                easing.type: Easing.OutBounce
            }
        }

    onDragEnded: {
        if (flickableWebList.contentY < -refreshItem.refreshHeight) {
            updateWebPageList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableWebList.contentY
    }

    Pane {
        id: mainPaneWebList
        width: parent.width
        padding: 0

        ColumnLayout {
            id: mainWebListColumnLayout
            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorWebList
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
                infoVisible: true
            }

            Column {
                id: columnLayoutWebList
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 5
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }

    }

    property bool isViewAlreadyOpened: false
    property int iCurrentCommandIndex: 0

    function pageOpenedUpdateView() {

        updateHeaderFromMain("Bautagebuch", "")

        if (!isViewAlreadyOpened)
            updateWebPageList();
        else
            showWebPageList();
        isViewAlreadyOpened = true;
    }

    function updateWebPageList() {
        gDataWebPageManager.startListWebPageData();
        busyIndicatorWebList.loadingVisible = true;
        busyIndicatorWebList.infoText = "Aktualisiere Liste"
        iCurrentCommandIndex = 1;
    }

    function notifyWebPageCommandFinished(result){
        if (iCurrentCommandIndex === 1) {   // list
            if (result === 1) {
                if (gDataAppUserEvents.getCurrentWebPageEventCounter() > 0) {
                    gConUserSettings.startGettingUserProps(true);
                    return;
                }
                toastManager.show("Liste erfolgreich geladen", 2000);
                busyIndicatorWebList.loadingVisible = false;
            } else {
                toastManager.show(userInt.getErrorCodeToString(result), 4000);
                busyIndicatorWebList.loadingVisible = false;
                busyIndicatorWebList.infoText = "Liste konnte nicht geladen werden"
            }
            showWebPageList();
        }
    }

    function notifyGetUserProperties(result) {
        if (result === 1) {
            toastManager.show("Liste erfolgreich geladen", 2000);
            busyIndicatorWebList.loadingVisible = false;
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorWebList.loadingVisible = false;
            busyIndicatorWebList.infoText = "Fehler beim Laden der Events"
        }

        showWebPageList();
    }

    function showWebPageList() {

        for (var j = columnLayoutWebList.children.length; j > 0; j--) {
            columnLayoutWebList.children[j-1].destroy()
        }

        if (gDataWebPageManager.getWebListLength() > 0) {
//            for (var i = gDataWebPageManager.getWebListLength() - 1; i >= 0; i--) {
            for (var i = 0; i < gDataWebPageManager.getWebListLength(); i++) {
                var sprite = webDataItem.createObject(columnLayoutWebList)
                sprite.showTextDataInfo(i, 1);
            }
            busyIndicatorWebList.infoText = "Letztes Update vor " + gDataWebPageManager.getWebPageLastLocalUpdateString()
        } else
            busyIndicatorWebList.infoText = "Keine Daten gespeichert\nZiehen zum Aktualisieren"

    }

    Component {
        id: webDataItem
        MyComponents.TextDataDesignItem {
            onClickedItem: {
                var component = Qt.createComponent("../pages/StadiumWebPage.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    sprite.startShowElements(sender, false);

//                    if (sender.event)
//                        gDataAppUserEvents.clearUserEventWebPage(sender.index);
                }

//                Qt.openUrlExternally(sender.info);
            }
        }
    }


    function notifyUserIntConnectionFinished(result, msg) {}
}
