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

Item {
    Pane {
       id: mainPaneLogging
       width: parent.width
       height: parent.height

           Text {
               id: txtForFontFamily
               visible: false
           }

           Item {
               width: parent.width
               anchors.top : parent.top
               anchors.bottom: rowConsoleCommand.top
               anchors.bottomMargin: 5

               Flickable {
                   anchors.fill: parent
                   flickableDirection: Flickable.VerticalFlick

                   TextArea.flickable: TextArea {
                       id: textAreaConsole
                       font.family: txtForFontFamily.font
                       width: parent.width
                       color: "#ffffff"
                       leftPadding: 5
                       rightPadding: 5
                       font.pixelSize: 11
                       wrapMode: TextEdit.Wrap
                       focus: true
                       enabled: false
                   }

                   ScrollIndicator.vertical: ScrollIndicator { }
               }
           }

       RowLayout {
           id: rowConsoleCommand
           width: parent.width
           anchors.bottom: parent.bottom

           MyComponents.EditableTextWithHint {
               id: textInput
               Layout.fillWidth: true
               hint: "Command"
               imageSource: ""
               enableImage: false
               enableKeyEnterSignal: true
               enabled: true
               color: "#FFFFFF"
               onKeysEnterPressed: gDataConsoleManager.startSendConsoleCommand(textInput.input)
           }

           MyComponents.GraphicalButton {
               imageSource: "../images/send.png"
               enabled:  true
               onClickedButton: {
                    gDataConsoleManager.startSendConsoleCommand(textInput.input)
               }
               Layout.alignment: Qt.AlignRight
           }
       }



   }

   function pageOpenedUpdateView() {

   }

   function notifyConsoleCommandFinished(result) {

       if (result === 1) {
           textAreaConsole.text = gDataConsoleManager.getLastConsoleOutput();
           toastManager.show("Kommando erfolgreich ausgeführt", 2000);
           textInput.clear();
       } else {
           toastManager.show(userIntCurrentNews.getErrorCodeToString(result), 4000);
       }
   }

   function notifyUserIntConnectionFinished(result, msg) {}


//   ScrollIndicator.vertical: ScrollIndicator { }
}
