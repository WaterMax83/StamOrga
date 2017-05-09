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
   width: 300
   contentHeight: mainPaneLogging.height
   Pane {
       id: mainPaneLogging
       width: parent.width

       Rectangle {
           color: "#3f3f3f"
           width: parent.width
           height: txtLogging.height
       Text {
           id: txtLogging
           width: parent.width
           text: "Hallo Welt, Dies ist ein Test \\n Wie läuft das mit Multiline ich will vesuchen hier einen richtig langen Text zu schreiben damit es am Ende überlauft"
           styleColor: "#ffffff"
           wrapMode: Text.WrapAtWordBoundaryOrAnywhere
           //           implicitWidth: mainPaneLogging.width
           Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
           rightPadding: 5
           visible: true
           color: "white"
       }
       }

   }



   ScrollIndicator.vertical: ScrollIndicator { }
}
