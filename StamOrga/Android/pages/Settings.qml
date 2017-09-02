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
                   font.pixelSize: 14
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
                   font.pixelSize: 14
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
                   font.pixelSize: 14
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
               visible: userInt.isDeviceMobile() || userInt.isDebuggingEnabled()

               Text {
                   id: labelNotificationText
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pixelSize: 14
                   Layout.topMargin: (saveInfosOnApp.height - height) / 2
                   Layout.bottomMargin: (saveInfosOnApp.height - height) / 2
                   color: "white"
                   text: "Benachrichtigungen >"
               }
               MouseArea {
                   anchors.fill: labelNotificationText
                   onClicked: {
                       var component = Qt.createComponent("../components/NotificationSettingsDialog.qml");
                       if (component.status === Component.Ready) {
                           var dialog = component.createObject(mainPaneSettings,{popupType: 1});
                           dialog.parentHeight = mainWindow.height
                           dialog.parentWidth = mainWindow.width
                           dialog.enableNewAppVersion = notifyNewAppVersion;
                           dialog.enableMeetingAdded = notifyNewMeetInfo;
                           dialog.enableMeetingChanged = notifyChangeMeetInfo;
                           dialog.enableNewFreeTicket = notifyFreeTicket;
                           dialog.enableNewAwayAccept = notifyAwayAccept;
                           dialog.acceptedDialog.connect(acceptedNotificationDialog);
                           notifyDialog = dialog;
                           dialog.open();
                       }
                   }
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
                   font.pixelSize: 14
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
                   font.pixelSize: 14
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
                   font.pixelSize: 14
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
               id: labelVersionText
               textFormat: Text.RichText
               wrapMode: Text.WordWrap
               Layout.maximumWidth: parent.width
               Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
               Layout.topMargin: (saveInfosOnApp.height - height) / 2
               Layout.bottomMargin: (saveInfosOnApp.height - height) / 2
               font.pixelSize: 14
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

        if (notifyNewAppVersion !== globalSettings.isNotificationNewAppVersionEnabled()) {
            globalSettings.setNotificationNewAppVersionEnabled(notifyNewAppVersion);
            saveSettings = true;
        }
        if (notifyNewMeetInfo !== globalSettings.isNotificationNewMeetingEnabled()){
            globalSettings.setNotificationNewMeetingEnabled(notifyNewMeetInfo);
            saveSettings = true;
        }
        if (notifyChangeMeetInfo !== globalSettings.isNotificationChangedMeetingEnabled()){
            globalSettings.setNotificationChangedMeetingEnabled(notifyChangeMeetInfo);
            saveSettings = true;
        }
        if (notifyFreeTicket !== globalSettings.isNotificationNewFreeTicketEnabled()){
            globalSettings.setNotificationNewFreeTicketEnabled(notifyFreeTicket);
            saveSettings = true;
        }
        if (notifyAwayAccept !== globalSettings.isNotificationNewAwayAcceptEnabled()){
            globalSettings.setNotificationNewAwayAcceptEnabled(notifyAwayAccept);
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

        cbfontFamilies.currentIndex = globalSettings.getCurrentFontIndex();

        notifyNewAppVersion = globalSettings.isNotificationNewAppVersionEnabled();
        notifyNewMeetInfo = globalSettings.isNotificationNewMeetingEnabled();
        notifyChangeMeetInfo = globalSettings.isNotificationChangedMeetingEnabled()
        notifyFreeTicket = globalSettings.isNotificationNewFreeTicketEnabled();
        notifyAwayAccept = globalSettings.isNotificationNewAwayAcceptEnabled();

       isStartupDone = true;
   }

   property var notifyNewAppVersion;
   property var notifyNewMeetInfo;
   property var notifyChangeMeetInfo;
   property var notifyFreeTicket;
   property var notifyAwayAccept;
   property var notifyDialog;
   function acceptedNotificationDialog() {
       notifyNewAppVersion = notifyDialog.enableNewAppVersion;
       notifyNewMeetInfo = notifyDialog.enableMeetingAdded;
       notifyChangeMeetInfo = notifyDialog.enableMeetingChanged;
       notifyFreeTicket = notifyDialog.enableNewFreeTicket;
       notifyAwayAccept = notifyDialog.enableNewAwayAccept;
       valueWasEditedEnableSave();
   }

   function notifyUserIntConnectionFinished(result) {}
}
