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
import QtQml.Models 2.1

import com.watermax.demo 1.0

import "../components" as MyComponents

Flickable {
    id: flickableCurrentMediaInfo
    clip: true
    height: parent.height * 1.2
    contentHeight: mainPaneCurrentMediaInfo.height
//    property bool useCommentLine : false

    signal showInfoHeader(var text, var load)

    flickableDirection: Flickable.VerticalFlick
    rebound: Transition {
        NumberAnimation {
            properties: "y"
            duration: 1000
            easing.type: Easing.OutBounce
        }
    }

    property int listViewItemHeight : 30

    onDragEnded: {
        if (flickableCurrentMediaInfo.contentY < -refreshItem.refreshHeight) {
//            loadAvailableTicketList();
        }
    }

    MyComponents.RefreshItem {
        id: refreshItem
        contentY: flickableCurrentMediaInfo.contentY
    }

    Pane {
        id: mainPaneCurrentMediaInfo
        width: parent.width

//        ColumnLayout {
//            id: mainColumnLayoutCurrentMediaInfo
//            width: parent.width

//            Text {
////                id: txtInfoCurrentGameFreeTickets
////                visible: columnLayoutFreeTickets.children.length > 0 ? true : false
//                color: "grey"
//                font.pixelSize: 14
//                text: "<b>Freie Karten:</b>"
//                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
//            }
//        }

        ListModel {
            id: photosModel
            ListElement { imageSrc: "../images/account.png" }
            ListElement { imageSrc: "../images/add.png" }
            ListElement { imageSrc: "../images/back.png" }
        }

//        DelegateModel {
//            id: visualModel; delegate: MyComponents.MediaDelegate { }
//            model: photosModel
////            Component.onCompleted: parts.selection
//        }

        GridView {
            id: photosGridView
            height: parent.height
            width: parent.width
            cellWidth: parent.width / 4
            cellHeight: parent.width / 4
            interactive: true
            model: photosModel
            delegate: Item {
                height: photosGridView.cellWidth
                width: photosGridView.cellHeight
                Image {
                    id: originalImage; antialiasing: true;
                    source: imageSrc; cache: false
                    fillMode: Image.PreserveAspectFit;
                    width: parent.width; height: parent.width;
                }

        //        Image {
        //            id: hqImage; antialiasing: true; source: imageSrc; visible: false; cache: false
        //            fillMode: Image.PreserveAspectFit; width: parent.width; height: parent.height
        //        }

                MouseArea {
                    width: originalImage.paintedWidth; height: originalImage.paintedHeight; anchors.centerIn: originalImage
                    onClicked: {
                        console.log("Clicked Image to open " + index)
                        var component = Qt.createComponent("../pages/MediaPage.qml")
                        if (component.status === Component.Ready) {
                            var sprite = stackView.push(component)
                            sprite.startWithIndex(index)
                        } else
                            console.error("Fehler beim Laden von der Medienseite " + component.errorString())
                    }
                }
            }
//            onCurrentIndexChanged: photosListView.positionViewAtIndex(currentIndex, ListView.Contain)
        }

//        Rectangle { id: photosShade; color: 'black'; width: parent.width; height: mainPaneCurrentGame.height; opacity: 0; visible: opacity != 0.0 }

//        ListView {
//            id: photosListView; model: visualModel.parts.list; orientation: Qt.Horizontal
//            width: parent.width; height: parent.height; interactive: false
//            onCurrentIndexChanged: photosGridView.positionViewAtIndex(currentIndex, GridView.Contain)
//            highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapOneItem
//        }

//        Item { id: foreground; anchors.fill: parent }
    }

    function showAllInfoAboutGame(sender) {

//        loadAvailableTicketList()
        console.log("mainPaneCurrentMediaInfo " + mainPaneCurrentMediaInfo.width +  " " + mainPaneCurrentMediaInfo.height)
        console.log("photosGridView " + photosGridView.width + " " + photosGridView.height)
    }
}
