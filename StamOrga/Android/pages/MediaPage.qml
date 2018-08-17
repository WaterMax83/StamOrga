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

Item {
    id: mainItemMediaPage
    width: mainWindow.width
    height: mainWindow.height

    ListView {
        id: photosListView; model: photosModel; orientation: Qt.Horizontal
        anchors.fill: parent; interactive: true
        onCurrentIndexChanged: console.log("Current Index " + index + " " + photosListView.currentIndex)
        highlightRangeMode: ListView.StrictlyEnforceRange; snapMode: ListView.SnapOneItem
        delegate: Rectangle {
            color: "black"
            width: mainItemMediaPage.width
            height: mainItemMediaPage.height

            Image {
                id: originalImage; antialiasing: true;
                source: imageSrc; cache: false
                fillMode: Image.PreserveAspectFit;
                anchors.fill: parent
            }
        }
    }

    function startWithIndex(index) {
        photosListView.positionViewAtIndex(index, ListView.Contain);
    }
}
