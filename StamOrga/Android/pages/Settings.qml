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

Item {
   id: itemSettings

   Component.onDestruction: {
       if (globalUserData.lastGamesLoadCount !== spBoxLoadPastGames.value) {
           globalUserData.lastGamesLoadCount = spBoxLoadPastGames.value;
           globalUserData.saveGlobalSettings();
       }
   }

   Pane {
       id: mainPaneSettings
       width: parent.width

       ColumnLayout {
           width: parent.width

           RowLayout {
//               width: parent.width
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true

               Label {
                   id: text1
                   text: qsTr("Lade beendete Spiele:")
                   Layout.fillWidth: true
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pixelSize: 14
                   color: "white"
               }
               SpinBox {
                   id: spBoxLoadPastGames
                   to: 50
                   from: 0
                   value: globalUserData.lastGamesLoadCount
                   Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
               }
           }
       }
   }

   function pageOpenedUpdateView() {
   }
}
