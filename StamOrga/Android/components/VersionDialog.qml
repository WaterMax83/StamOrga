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
import "../components" as MyComponents

MyComponents.CustomDialog {
    property int parentWidth : 540
    property int parentHeight : 960
    property alias versionText: versionDialogTextUpdate.text
    id: versionDialog
    modal: true
    focus: true
    title: "Neue Version"
    x: (parentWidth - width) / 2
    y: parentHeight / 6
    width: Math.min(parentWidth, parentHeight) / 3 * 2
        contentHeight: versionColumn.height

    Column {
        id: versionColumn
        spacing: 20

        Text {
            width: versionDialog.availableWidth
            text: "Es gibt eine neue Version von StamOrga"
            wrapMode: Text.Wrap
            color: "white"
            font.pixelSize: 12
        }

        Text {
            id: versionDialogTextUpdate
            width: versionDialog.availableWidth
            textFormat: Text.RichText
            wrapMode: Text.Wrap
            font.pixelSize: 12
            color: "white"
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
}
