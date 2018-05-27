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
import QtGraphicalEffects 1.0

import com.watermax.demo 1.0

Rectangle {
    id: refreshRectangle
    property int refreshHeight: 60
    property int contentY: 0
    Image {
        id: refreshImage
        source: "../images/refresh.png"
        rotation: (contentY > - refreshHeight) ? (contentY  * 4 ): -240
        transformOrigin: Item.Center
        opacity: (contentY * -1) / refreshHeight
        anchors.centerIn: parent
    }
    ColorOverlay {
        anchors.fill: refreshImage
        source: refreshImage
        rotation: refreshImage.rotation
        opacity: refreshImage.opacity
        color: "#2196F3"
    }

    visible: contentY < 0 ? true : false
    color: "#536878"
    width: refreshImage.width * 2
    height: refreshImage.height * 2
    radius: width * 0.5
    y: contentY + 10
    x: (parent.width / 2) - (width / 2)
    z: 1000
}
