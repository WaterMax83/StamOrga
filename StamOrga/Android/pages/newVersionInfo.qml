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


import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Flickable {
    id: flickableNewVersionInfo
    contentHeight: mainPaneNewVersionInfo.height
    boundsBehavior: Flickable.StopAtBounds


    Pane {
        id: mainPaneNewVersionInfo
        width: parent.width


        ColumnLayout {
                id: accepptTextDialogColumn
                width: parent.width
                spacing: 20

                Text {
                    id: labelAcceptTextHeader
                    wrapMode: Text.WordWrap
                    Layout.maximumWidth: parent.width
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: "Was ist neu:"
                    color: "white"
                    font.bold: true
                    font.pointSize: 18
                }

                Text {
                    id: labelAcceptText
                    textFormat: Text.RichText
                    wrapMode: Text.WordWrap
                    Layout.maximumWidth: parent.width
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pointSize: 12
                    color: "white"
                    text: globalSettings.getVersionChangeInfo()
                }
            }
    }

    function pageOpenedUpdateView() {

    }
}
