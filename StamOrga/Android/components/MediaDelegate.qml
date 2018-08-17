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

import QtQuick 2.0

Package {
    Item { id: listItem; Package.name: 'list'; width: photosShade.width; height: photosShade.height; }
    Item { id: gridItem; Package.name: 'grid'; width: photosGridView.cellWidth; height: photosGridView.cellHeight;}

    Item {
        id: mediaWrapper
        parent: gridItem
        height: parent.width
        width: parent.width
        x: 0
        y: 0
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
                if (mediaWrapper.state == 'fullscreen') {
                    gridItem.GridView.view.currentIndex = index;
                    mediaWrapper.state = 'inGrid'
                } else {
                    gridItem.GridView.view.currentIndex = index;
                    mediaWrapper.state = 'fullscreen'
                }
            }
        }

        states: [
            State {
                name: 'inGrid';
                ParentChange { target: mediaWrapper; parent: gridItem; }
                PropertyChanges { target: photosGridView; interactive: true }
                PropertyChanges { target: photosListView; interactive: false }
                PropertyChanges { target: photosShade; opacity: 0 }
            },
            State {
                name: 'fullscreen';
                ParentChange {
                    target: mediaWrapper; parent: listItem; x: 0; y: 0;
                    width: listItem.width; height: listItem.height
                }
//                PropertyChanges { target: border; opacity: 0 }
//                PropertyChanges { target: hqImage; source: listItem.ListView.isCurrentItem ? imageSrc : ""; visible: true }
                PropertyChanges { target: photosGridView; interactive: false }
                PropertyChanges { target: photosListView; interactive: true }
                PropertyChanges { target: photosShade; opacity: 1 }
            }
        ]
        transitions: [
            Transition {
                from: '*'; to: 'fullscreen'
                SequentialAnimation {
                    PauseAnimation { duration: gridItem.GridView.isCurrentItem ? 0 : 600 }
                    ParentAnimation {
                        target: mediaWrapper; via: foreground
                        NumberAnimation {
                            targets: [ mediaWrapper ]
                            properties: 'x,y,width,height,opacity'
                            duration: gridItem.GridView.isCurrentItem ? 600 : 1; easing.type: 'OutQuart'
                        }
                    }
                }
            }
//            Transition {
//                from: 'fullscreen'; to: 'inGrid'
//                ParentAnimation {
//                    target: mediaWrapper; via: foreground
//                    NumberAnimation {
//                        targets: [ mediaWrapper ]
//                        properties: 'x,y,width,height,opacity'
//                        duration: gridItem.GridView.isCurrentItem ? 600 : 1; easing.type: 'OutQuart'
//                    }
//                }
//            }
        ]
    }
}
