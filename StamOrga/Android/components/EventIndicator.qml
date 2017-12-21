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
    property int eventCount;
    property bool disableVisibility : false;
    property int itemSize : parent.height

    visible: disableVisibility ? false : ((eventCount > 0) ? true : false)
    width: itemSize / 2
    height: itemSize / 2
    radius: width*0.5
    anchors.top: parent.top
    anchors.right: parent.right
    color: "red"
    Text {
          id: txtNumberToolButtonEventCount
          anchors.verticalCenter: parent.verticalCenter
          anchors.horizontalCenter: parent.horizontalCenter
          color: "white"
          text: eventCount
    }
}
