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
    property alias hint: inputText.placeholderText
    property alias input: inputText.text
    property alias imageSource: rightImage.source
    property bool  enableImage: false

    signal textInputChanged();
    signal keysEnterPressed();

    implicitWidth: parent.width
    implicitHeight: inputText.height > 0 ? inputText.height : 30
//    Text {
//        id: hintText
//        anchors { fill: parent; leftMargin: 14 }
//        verticalAlignment: Text.AlignVCenter
//        color: "#707070"
//        opacity: inputText.displayText.length ? 0 : 1
//    }

    TextArea {
        id: inputText
//        font.family: hintText.font
        anchors { fill:parent; leftMargin: 5 }
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 16
        color: "#505050"
        Layout.fillWidth: true
        wrapMode: Text.Wrap
        layer.enabled: true
        onTextChanged: {
            if (isInit)
                return;
            textInputChanged();
        }
        Keys.onReturnPressed: keysEnterPressed();
    }

    Image {
        id: rightImage
        width: enableImage ? parent.height : 0
        height: parent.height
        anchors.right: parent.right
    }
//    ColorOverlay {
//        anchors.fill: rightImage
//        source: rightImage
//        color: "#2196F3"
//    }

    property bool isInit: false

    function init(text) {
        isInit = true;
        inputText.text = text;
        isInit = false;
    }

    function clear() {
        inputText.text = "";
    }
}


