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
    property alias hint: hintText.text
    property alias input: inputText.text
    property alias imageSource: rightImage.source
    property bool  enableImage: false
    property bool enableKeyEnterSignal : false

    property int minHeight: 30

    signal textInputChanged();
    signal keysEnterPressed();


    implicitWidth: parent.width
    implicitHeight: Math.max(inputText.height, minHeight)
    Text {
        id: hintText
        anchors { fill: parent; leftMargin: 14 }
        verticalAlignment: Text.AlignVCenter
        color: "grey"
//        opacity: inputText.text.length ? 0 : 1
        visible: inputText.focus ? false : inputText.length > 0 ? false : true
    }

    TextEdit {
        id: inputText
        font.family: hintText.font
        width: parent.width
        topPadding: 2
        bottomPadding: 2
        leftPadding: 5
        rightPadding: rightImage.width + 5
        anchors.centerIn: parent
        font.pixelSize: 14
        color: parent.enabled ? "#505050" : "#B0BEC5"
        wrapMode: Text.Wrap
        textFormat: Text.PlainText
        onTextChanged: {
//            if (text.length > 0)
//                hintText.visible = false
//            else
//                hintText.visible = true
            if (isInit)
                return;
            textInputChanged();
        }
        Keys.onReturnPressed: {
            if (enableKeyEnterSignal)
                keysEnterPressed();
            else
                inputText.append("")
        }
    }

    Image {
        id: rightImage
        width: enableImage ? minHeight : 0
        height: minHeight
        anchors.right: parent.right
    }
    ColorOverlay {
        anchors.fill: rightImage
        source: rightImage
        color: "#2196F3"
    }

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


