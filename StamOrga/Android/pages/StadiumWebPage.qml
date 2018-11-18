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
import QtWebView 1.1

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {

    property var textDataItem
//    anchors.fill: parent

//    Pane {
//        width: parent.width
//        height: parent.height

//        Text {
//            id: txt123
//            visible: true
//            text: "Hello"
//        }


//        Rectangle {
//            width: parent.width
//            height: parent.height
//                color: "blue"
//        }
//    }

//    ScrollView {
//        anchors.fill: parent

        WebView {
            id: webView
            anchors.fill: parent
//            url: "qt.io"
            onLoadingChanged: {
                console.log("Loading changed");
            }
        }
//    }

    function pageOpenedUpdateView() {
//        webView.reload();
    }

    function startShowElements(textItem, editMode) {

        textDataItem = textItem;

        if (textItem !== undefined) {
            gDataWebPageManager.startLoadWebPage(textItem.index)
//            isStartupDone = false;
        }
        updateHeaderFromMain(textItem.timestampReadableLine(), "")
    }

    function notifyWebPageCommandFinished(result){
        if (result === 1) {
            console.log("Laden hat funkioniert");
            toastManager.show("Seite erfolgreich geladen", 2000);
            if (textDataItem !== undefined)
                webView.loadHtml(textDataItem.info);
        } else {
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
        }
    }
}
