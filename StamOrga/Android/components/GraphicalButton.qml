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

Rectangle {
    property alias imageSource: imageButton.source

    signal clickedButton();

    id: rectButton
    width: 30
    height: 30
    color: enabled ? "#2196F3" : "BlueGrey"
    radius: 8
    onEnabledChanged: {
        if (enabled)
            color = "#2196F3";
        else
            color = "BlueGrey"
    }
    Image {
        id: imageButton
        anchors.fill: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            animateButton.stop()
            if (rectButton.enabled) {
                rectButton.color = "#2196F3"
                clickedButton();
            }
        }
        onPressed: {
            if (rectButton.enabled)
                animateButton.start()
        }
    }
    ColorAnimation {
        id: animateButton
        target: rectButton
        property: "color"
        from: "#2196F3"
        to: "LightBlue"
        duration: 250
    }
}
