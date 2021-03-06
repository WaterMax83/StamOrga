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

//    ScrollView {
//        anchors.fill: parent

        WebView {
            id: webView
            anchors.fill: parent
            onCanGoBackChanged: console.log ("Back changed")
//            url: "qt.io"
        }
//    }

    function pageOpenedUpdateView() { }

    function toolButtonClicked(){
        gDataWebPageManager.startSetWebPage(textDataItem.header, textDataItem.info);
    }

    function startShowElements(textItem, editMode) {

        textDataItem = textItem;

        if (textItem !== undefined) {
            gDataWebPageManager.startLoadWebPage(textItem.index, mainWindow.width)
        }
        if (gConUserSettings.userIsAdminEnabled())
            updateHeaderFromMain(textItem.header, "/images/refresh.png")
        else
            updateHeaderFromMain(textItem.header, "")
    }

    function notifyWebPageCommandFinished(result, subCmd){
        if (subCmd === 1) { // load
            if (result === 1) {
                toastManager.show("Seite wird geladen", 2000);
                if (textDataItem !== undefined) {
                    if (userInt.isStringUrl(textDataItem.info)) {
                        webView.url = textDataItem.info
                        webView.reload();
                    } else
                        webView.loadHtml(textDataItem.info);
                }
            } else {
                toastManager.show(userInt.getErrorCodeToString(result), 4000);
            }
        } else if (subCmd === 2) {// set
            if (result === 1)
                toastManager.show("Seite wurde gespeichert", 2000);
            else
                toastManager.show(userInt.getErrorCodeToString(result), 4000);
        }
    }
}
