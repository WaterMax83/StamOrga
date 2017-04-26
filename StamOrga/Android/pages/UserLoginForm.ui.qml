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

       Text {
           id: txtLogging
           text: "Hallo Welt, Dies ist ein Test \\n Wie läuft das mit Multiline"
           wrapMode: Text.WrapAnywhere
           //           implicitWidth: mainPaneLogging.width
           Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
           rightPadding: 5
           visible: true
       }

   }



   ScrollIndicator.vertical: ScrollIndicator { }
}
