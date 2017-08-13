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

Flickable {
   id: flickableLogging
   contentHeight: mainPaneLogging.height
//   width: parent.width
   Pane {
       id: mainPaneLogging
       width: parent.width

       ColumnLayout {
           width: parent.width
           spacing: 5
           ComboBox {
                id: logFilesCombo
               model: globalUserData.getCurrentLogFileList()
               anchors.horizontalCenter: parent.horizontalCenter
               onCurrentIndexChanged: {
                   txtLogging.text = globalUserData.getCurrentLoggingList(logFilesCombo.currentIndex);
               }
           }

           Text {
               id: txtLogging
               wrapMode: Text.WordWrap
               Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               Layout.maximumWidth: parent.width
               rightPadding: 5
               visible: true
               font.pointSize: 8
               color: "white"
           }

           Button {
               text: "Kopieren"
               implicitWidth: parent.width / 3 * 2
               Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               onClicked: {
                    globalUserData.copyTextToClipBoard(txtLogging.text);
                    toastManager.show("Daten wurden in die Zwischenablage kopiert", 2000);
               }
           }

           Button {
               text: "Löschen"
               implicitWidth: parent.width / 3 * 2
               Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               onClicked: {
                    globalUserData.deleteCurrentLoggingFile(logFilesCombo.currentIndex);
                    pageOpenedUpdateView();
               }
           }
       }
   }

   function pageOpenedUpdateView() {

       if (logFilesCombo.count > 0)
            logFilesCombo.currentIndex = logFilesCombo.count - 1
       else {
           logFilesCombo.visible = false;
           txtLogging.text = globalUserData.getCurrentLoggingList(0);
       }
   }

   function notifyUserIntConnectionFinished(result) {}


   ScrollIndicator.vertical: ScrollIndicator { }
}
