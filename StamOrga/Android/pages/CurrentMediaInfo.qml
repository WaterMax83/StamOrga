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

    Rectangle {
        width: 60
        height: 60
        radius: width * 0.5
        color : "#2196F3"
        x: parent.width - width - 20
        y: flickableCurrentMediaInfo.parent.height - height - 20 + flickableCurrentMediaInfo.contentY
        z: 1000
        Image {
            id: addImage
            width: parent.width / 3 * 2
            height: parent.width / 3 * 2
            source: "../images/add.png"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var componentFile = Qt.createComponent("../components/PictureFileDialog.qml")
                if (componentFile.status === Component.Ready) {
                    var dialog = componentFile.createObject(mainPaneCurrentGame);
                    dialog.acceptedFileDialog.connect(acceptedFileDialogMedia);
                    dialog.open();
                } else
                    console.error("Fehler beim Laden des File Dialog " + componentFile.errorString())
            }
        }
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
            //            ListElement { imageSrc: "../images/account.png" }
            //            ListElement { imageSrc: "../images/add.png" }
            //            ListElement { imageSrc: "../images/back.png" }
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
                        var componentPage = Qt.createComponent("../pages/MediaPage.qml")
                        if (componentPage.status === Component.Ready) {
                            var spritePage = stackView.push(componentPage)
                            spritePage.startWithIndex(index)
                        } else
                            console.error("Fehler beim Laden von der Medienseite " + componentPage.errorString())
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

    property int iCurrentCommandIndex: 0
    function showAllInfoAboutGame(sender) {

        //        loadAvailableTicketList()
        gDataMediaManager.startGetPictureList(m_gamePlayCurrentItem.index);
        iCurrentCommandIndex = 1;
    }

    function acceptedFileDialogMedia(url) {
        console.log("Try to add " + url)
        var result = gDataMediaManager.startAddPicture(m_gamePlayCurrentItem.index, url);
        if (result !== 1)
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
        else
            iCurrentCommandIndex = 2;
    }

    function notifyMediaCommandFinished(result){
        if (iCurrentCommandIndex === 1) {   // list
            if (result === 1)
                toastManager.show("Medien geladen", 2000);
            else if (result === -5)
                toastManager.show("Bisher noch keine Medien gespeichert", 2000);
            else {
                toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
                showInfoHeader("Medien laden hat nicht funktioniert", false)
            }
        } else if (iCurrentCommandIndex === 2) { // add
            if (result === 1) {
                toastManager.show("Bild erfolgreich hinzugefügt", 2000);
                gDataMediaManager.startGetPictureList(m_gamePlayCurrentItem.index);
            } else {
                toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
                showInfoHeader("Bild hinzufügen hat nicht funktioniert", false)
            }
        }
    }
}
