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

   Pane {
       id: mainPaneSettings
       width: parent.width

       ColumnLayout {
           width: parent.width
           spacing: 20

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true

               Text {
                   id: text2
                   text: qsTr("Benutze Nutzername:")
                   Layout.fillWidth: true
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   color: "white"
                   font.pointSize: 12
               }
               CheckBox {
                    id: useReadableName
                    checked: globalSettings.useReadableName
                    onCheckedChanged: valueWasEditedEnableSave()
               }
           }

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true

               Text {
                   id: text3
                   text: qsTr("Aktualisiere Spiele automatisch:")
                   Layout.fillWidth: true
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pointSize: 12
                   color: "white"
               }
               CheckBox {
                    id: loadGameInfo
                    checked: globalSettings.loadGameInfo
                    onCheckedChanged: valueWasEditedEnableSave()
               }
           }

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true

               Text {
                   id: text4
                   text: qsTr("Speichere geladene Daten:")
                   Layout.fillWidth: true
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pointSize: 12
                   color: "white"
               }
               CheckBox {
                    id: saveInfosOnApp
                    checked: globalSettings.saveInfosOnApp
                    onCheckedChanged: valueWasEditedEnableSave()
               }
           }

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true
               visible: !userInt.isDeviceMobile()

               Text {
                   id: text5
                   text: qsTr("Schrift:")
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pointSize: 12
                   color: "white"
               }
               ComboBox {
                   id: cbfontFamilies
                   Layout.fillWidth: true
                   model: fontFamiliesModel
                   textRole: "display"
                   onCurrentIndexChanged: valueWasEditedEnableSave();
               }

           }

           RowLayout {
               spacing: 5
               width: parent.width
               Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               visible: userInt.isDebuggingEnabled()

               Text {
                   text: qsTr("Alternative IP:")
                   font.pointSize: 12
                   color: "white"
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
               }

               TextField {
                   id: txtOtherIPAddr
                   text: globalSettings.debugIP
                   implicitWidth: parent.width
                   Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                   Layout.fillWidth: true
                   onTextChanged: valueWasEditedEnableSave()
               }
           }

           RowLayout {
               spacing: 5
               width: parent.width
               Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               visible: userInt.isDebuggingEnabled() && userInt.isDeviceMobile()

               Text {
                   text: qsTr("Alternative IP Wlan")
                   font.pointSize: 12
                   color: "white"
                   Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
               }

               TextField {
                   id: txtOtherIPAddrWifi
                   text: globalSettings.debugIPWifi
                   implicitWidth: parent.width
                   Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                   Layout.fillWidth: true
                    onTextChanged: valueWasEditedEnableSave()
               }
           }

           Text {
               id: labelAcceptText
               textFormat: Text.RichText
               wrapMode: Text.WordWrap
               Layout.maximumWidth: parent.width
               Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
               font.pointSize: 12
               color: "white"
               text: "Aktuelle Version: <a href=\"tmp\">" + globalSettings.getCurrentVersion() + "</a>"
               onLinkActivated: {
                   var component = Qt.createComponent("../pages/newVersionInfo.qml");
                   if (component.status === Component.Ready) {
                       stackView.push(component);
                   }
               }
           }
       }
   }

    function toolButtonClicked() {
        var saveSettings = false;
        var saveFonts = false;

        if (globalSettings.useReadableName !== useReadableName.checked) {
           globalSettings.useReadableName = useReadableName.checked;
           saveSettings = true;
        }

        if (globalSettings.loadGameInfo !== loadGameInfo.checked) {
           globalSettings.loadGameInfo = loadGameInfo.checked;
           saveSettings = true;
        }

        if (globalSettings.saveInfosOnApp !== saveInfosOnApp.checked) {
           globalSettings.saveInfosOnApp = saveInfosOnApp.checked;
           saveSettings = true;
        }

        if (globalSettings.getChangeDefaultFont() !== cbfontFamilies.currentText ) {
            globalSettings.setChangeDefaultFont(cbfontFamilies.currentText);
            saveSettings = true;
            saveFonts = true;
        }

        if (globalSettings.debugIP !== txtOtherIPAddr.text) {
           globalSettings.debugIP = txtOtherIPAddr.text;
           saveSettings = true;
        }

        if (globalSettings.debugIPWifi !== txtOtherIPAddrWifi.text) {
           globalSettings.debugIPWifi = txtOtherIPAddrWifi.text;
           saveSettings = true;
        }

        if (saveSettings)
          globalSettings.saveGlobalSettings();

        updateHeaderFromMain("", "");

        isSaveButtonShown = false;

        toastManager.show("Daten erfolgreich gespeichert", 3000)

        if (saveFonts) {
            var component = Qt.createComponent("../components/AcceptDialog.qml");
            if (component.status === Component.Ready) {
              var dialog = component.createObject(mainPaneSettings,{popupType: 1});
              dialog.headerText = "Information";
              dialog.parentHeight = mainWindow.height
              dialog.parentWidth = mainWindow.width
              dialog.textToAccept = "Zum Aktualisieren der Schrift muss die App neu gestarted werden";
              dialog.showCancelButton = false
              dialog.open();
            }
        }
    }

   property bool isSaveButtonShown : false;
   property bool isStartupDone : false
   function valueWasEditedEnableSave() {

       if (!isStartupDone)
           return;

       if (isSaveButtonShown)
           return;

       isSaveButtonShown = true;
       updateHeaderFromMain("", "images/save.png");
   }

   function pageOpenedUpdateView() {

//       fontFamiliesModel.get(cbfontFamilies.currentIndex).text = "Arial"
        cbfontFamilies.currentIndex = globalSettings.getCurrentFontIndex();

       isStartupDone = true;
   }

   function notifyUserIntConnectionFinished(result) {}
}
