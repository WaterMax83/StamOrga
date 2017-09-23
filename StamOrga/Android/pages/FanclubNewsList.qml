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

import "../components" as MyComponents

Flickable {
    id: flickableFanclubNewsList
//    property UserInterface userIntTicket
//    flickingHorizontally: false
//    flickingVertically: true
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
        if (flickableFanclubNewsList.contentY < -100) {
            updateFanclubNewsList();
        }
    }

    Rectangle {
        Image {
            id: refreshImage
            source: "../images/refresh.png"
            rotation: (flickableFanclubNewsList.contentY > -100) ? (flickableFanclubNewsList.contentY * -1) * 2 : 220
            transformOrigin: Item.Center
        }
        opacity: (flickableFanclubNewsList.contentY * -1) / 100
        color: "black"
        width: refreshImage.width
        height: refreshImage.height
        radius: width * 0.5
        y: 50
        x: (mainWindow.width / 2) - (width / 2)
        z: 1000
    }

    Pane {
        id: mainPaneFanClubNewsList
        width: parent.width
        height: parent.height

        ColumnLayout {
            id: mainFanClubColumnLayout
            width: parent.width
            spacing: 10

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorNewsList
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                infoVisible: true
            }

            Column {
                id: columnLayoutFanClubList
                anchors.right: parent.right
                anchors.left: parent.left
                width: parent.width
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            }
        }

    }

    property bool isViewAlreadyOpened: false
    function pageOpenedUpdateView() {

        if (globalUserData.userIsFanclubEditEnabled() ||  userInt.isDebuggingEnabled())
            updateHeaderFromMain("Fanclub", "images/add.png")
        else
            updateHeaderFromMain("Fanclub", "")

        if (!isViewAlreadyOpened)
            updateFanclubNewsList();
        isViewAlreadyOpened = true;
        showNewsDataList();
    }

    function toolButtonClicked() {
        if (!globalUserData.userIsFanclubEditEnabled() &&  !userInt.isDebuggingEnabled())
            return;

        var component = Qt.createComponent("../pages/FanclubNewsItem.qml")
        if (component.status === Component.Ready) {
            var sprite = stackView.push(component)
            sprite.userIntCurrentNews = userInt
            sprite.startShowElements(undefined, true);
        }
    }

    function updateFanclubNewsList() {
        userInt.startListFanclubNews();
        busyIndicatorNewsList.loadingVisible = true;
        busyIndicatorNewsList.infoText = "Aktualisiere Liste"
    }

    function notifyFanclubNewsListFinished(result){
        if (result === 1) {
            toastManager.show("Liste erfolgreich geladen", 2000);
            busyIndicatorNewsList.loadingVisible = false;
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
            busyIndicatorNewsList.loadingVisible = false;
            busyIndicatorNewsList.infoText = "Liste konnte nicht geladen werden"
        }
        showNewsDataList();
    }

    function showNewsDataList() {

        for (var j = columnLayoutFanClubList.children.length; j > 0; j--) {
            columnLayoutFanClubList.children[j-1].destroy()
        }

        if (globalUserData.getNewsDataItemLength() > 0) {
            for (var i = 0; i < globalUserData.getNewsDataItemLength(); i++) {
                var sprite = newsDataItem.createObject(columnLayoutFanClubList)
                sprite.showNewsDataInfo(i);
            }
            busyIndicatorNewsList.infoText = "Letztes Update am " + globalUserData.getNewsDataLastLocalUpdateString()
        } else
            busyIndicatorNewsList.infoText = "Keine Daten gespeichert\nZiehen zum Aktualisieren"

    }

    Component {
        id: newsDataItem
        MyComponents.NewsDataItem {
            onClickedItem: {
                var component = Qt.createComponent("../pages/FanclubNewsItem.qml")
                if (component.status === Component.Ready) {
                    var sprite = stackView.push(component)
                    sprite.userIntCurrentNews = userInt
                    sprite.startShowElements(sender, false);
                }
            }
            onPressAndHold: {

            }
        }
    }

    function notifyUserIntConnectionFinished(result) {}
}
