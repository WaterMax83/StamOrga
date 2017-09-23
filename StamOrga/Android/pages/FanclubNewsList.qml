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
    }

    function toolButtonClicked() {
        if (!globalUserData.userIsFanclubEditEnabled() &&  !userInt.isDebuggingEnabled())
            return;

        var component = Qt.createComponent("../pages/FanclubNewsItem.qml")
        if (component.status === Component.Ready) {
            var sprite = stackView.push(component)
            sprite.userIntCurrentNews = userInt
            sprite.startShowElements(true);
        }
    }

    function updateFanclubNewsList() {
        userInt.startListFanclubNews();
        busyIndicatorNewsList.loadingVisible = true;
        busyIndicatorNewsList.infoVisible = true;
        busyIndicatorNewsList.infoText = "Aktualisiere Liste"
    }

    function notifyUserIntConnectionFinished(result) {}
}
