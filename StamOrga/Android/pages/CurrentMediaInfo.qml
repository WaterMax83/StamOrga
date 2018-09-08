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

    signal showInfoHeader(var text, var load)

    flickableDirection: Flickable.VerticalFlick
    rebound: Transition {
        NumberAnimation {
            properties: "y"
            duration: 1000
            easing.type: Easing.OutBounce
        }
    }

    onDragEnded: {
        if (flickableCurrentMediaInfo.contentY < -refreshItem.refreshHeight) {
            loadMediaList();
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
            source: bIsDeletingMode ? "../images/delete.png" : "../images/add.png"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (bIsDeletingMode) {
                    while (deleteArray.length > 0)
                        deleteArray.pop();
                    for(var i = 0; i < photosModel.count; i++) {
                        if (photosModel.get(i).deleteFlag) {
                            deleteArray.push(photosModel.get(i).imageSrc);
                        }
                    }

                    if (deleteArray.length === 0) {
                        toastManager.show("Keine Bilder ausgewählt", 2000);
                        return;
                    }

                    gDataMediaManager.startDeletePictures(m_gamePlayCurrentItem.index, deleteArray);
                    iCurrentCommandIndex = 3;
                    if (deleteArray.length === 1)
                        showInfoHeader("Lösche Bild", true)
                    else
                        showInfoHeader("Lösche Bilder", true)
                } else {

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
    }

    property var deleteArray: []

    Pane {
        id: mainPaneCurrentMediaInfo
        width: parent.width

        ListModel {
            id: photosModel
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
                Rectangle {
                    anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 5
                    width: 20; height: 20; radius: width * 0.5
                    color: "grey"; visible: deleteFlag
                    Image {
                        source: "../images/done.png"
                        fillMode: Image.PreserveAspectFit;
                        anchors.fill: parent
                    }
                }

                //        Image {
                //            id: hqImage; antialiasing: true; source: imageSrc; visible: false; cache: false
                //            fillMode: Image.PreserveAspectFit; width: parent.width; height: parent.height
                //        }

                MouseArea {
                    width: originalImage.paintedWidth; height: originalImage.paintedHeight; anchors.centerIn: originalImage
                    onClicked: {
                        if (bIsDeletingMode) {
                            if (photosModel.get(index).deleteFlag)
                                photosModel.get(index).deleteFlag = false
                            else
                                photosModel.get(index).deleteFlag = true
                        } else {

                            console.log("Clicked Image to open " + index)
                            var componentPage = Qt.createComponent("../pages/MediaPage.qml")
                            if (componentPage.status === Component.Ready) {
                                var spritePage = stackView.push(componentPage)
                                spritePage.startWithIndex(index)
                            } else
                                console.error("Fehler beim Laden von der Medienseite " + componentPage.errorString())
                        }
                    }
                    onPressAndHold: {
                        if (bIsDeletingMode) {
                            bIsDeletingMode = false
                            for(var i = 0; i < photosModel.count; i++)
                                photosModel.get(i).deleteFlag = false;
                        } else {
                            bIsDeletingMode = true;
                            photosModel.get(index).deleteFlag = true;
                        }
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
    property bool  bIsDeletingMode: false
    function showAllInfoAboutGame(sender) {

        loadMediaList()
    }

    function getPreventEscape() {
        if (bIsDeletingMode) {
            bIsDeletingMode = false;
            for(var i = 0; i < photosModel.count; i++)
                photosModel.get(i).deleteFlag = false;
            return true;
        }

        return false;
    }

    function loadMediaList() {
        gDataMediaManager.startGetPictureList(m_gamePlayCurrentItem.index);
        iCurrentCommandIndex = 1;
        showInfoHeader("Lade Bilder", true)
    }

    function acceptedFileDialogMedia(url) {
        var result = gDataMediaManager.startAddPicture(m_gamePlayCurrentItem.index, url);
        if (result !== 1)
            toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
        else {
            iCurrentCommandIndex = 2;
            showInfoHeader("Speichere Bild", true)
        }
    }

    function notifyMediaCommandFinished(result){
        if (iCurrentCommandIndex === 1) {   // list
            if (result === 1) {
                toastManager.show("Bilder geladen", 2000);

                photosModel.clear();
                for(var i = 0; i < gDataMediaManager.getMediaCount(); i++) {
                    var url = gDataMediaManager.getMediaFileString(i);
                    photosModel.append({ imageSrc: "image://media/" + url, deleteFlag: false});
                }
                showInfoHeader("", false)
            } else if (result === -5) {
                toastManager.show("Bisher noch keine Bilder gespeichert", 2000);
                showInfoHeader("", false)
            } else {
                toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
                showInfoHeader("Bilder laden hat nicht funktioniert", false)
            }
        } else if (iCurrentCommandIndex === 2) { // add
            if (result === 1) {
                toastManager.show("Bild erfolgreich hinzugefügt", 2000);
                loadMediaList();
            } else {
                toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
                showInfoHeader("Bild hinzufügen hat nicht funktioniert", false)
            }
        } else if (iCurrentCommandIndex === 3) { // delete
            if (result === 1) {
                toastManager.show("Bilder erfolgreich gelöscht", 2000);
                loadMediaList();
            } else {
                toastManager.show(userIntGames.getErrorCodeToString(result), 4000);
                showInfoHeader("Bild löschen hat nicht funktioniert", false)
            }
        }
    }
}
