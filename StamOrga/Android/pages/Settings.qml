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

Flickable {
   id: flickableSettings

   contentHeight: mainPaneSettings.height

   boundsBehavior: Flickable.StopAtBounds

   Pane {
       id: mainPaneSettings
       width: parent.width

       ColumnLayout {
           width: parent.width
           spacing: 20

           Text {
               id: txtForFontFamily
               visible: false
           }

//           RowLayout {
//               Layout.preferredWidth: parent.width
//               Layout.fillWidth: true

//               Text {
//                   id: text2
//                   text: qsTr("Benutze Nutzername:")
//                   Layout.fillWidth: true
//                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
//                   color: "white"
//                   font.pixelSize: 14
//               }
//               CheckBox {
//                    id: useReadableName
//                    checked: globalSettings.useReadableName
//                    onCheckedChanged: valueWasEditedEnableSave()
//               }
//           }

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
                    checked: gStaGlobalSettings.getLoadGameInfos()
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
                    checked: gStaGlobalSettings.getSaveInfosOnApp()
                    onCheckedChanged: valueWasEditedEnableSave()
               }
           }

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true

               Text {
                   id: text5
                   text: qsTr("Nutze Popup als Versionsinfo:")
                   Layout.fillWidth: true
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pixelSize: 14
                   color: "white"
               }
               CheckBox {
                    id: usePopupForVersionInfo
                    checked: gStaGlobalSettings.getUseVersionPopup()
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
                           dialog.enableFanclubNews = notifyFanclubNews;
                           dialog.visibleFanclubNews = gConUserSettings.userIsFanclubEnabled();
                           dialog.acceptedDialog.connect(acceptedNotificationDialog);
                           dialog.font.family= txtForFontFamily.font
                           notifyDialog = dialog;
                           dialog.open();
                       }
                   }
               }
           }

           RowLayout {
               Layout.preferredWidth: parent.width
               Layout.fillWidth: true
//               visible: !userInt.isDeviceMobile() || userInt.isDebuggingEnabled()
               visible: true

               Text {
                   id: textFontFamilies
                   text: qsTr("Schrift:")
                   Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                   font.pixelSize: 14
                   color: "white"
               }
               ComboBox {
                   id: cbfontFamilies
                   Layout.fillWidth: true
                   model: fontFamiliesModel
//                   model: Qt.fontFamilies()
                   textRole: "display"
//                   textRole: "text"
                   onCurrentIndexChanged: valueWasEditedEnableSave();
//                   delegate:  MenuItem {
//                               height: 40; width: cbfontFamilies.width
//                               Text {
//                                   anchors.centerIn: parent
//                                   text: modelData
//                                   font.family: modelData
//                                   font.pixelSize: 20
//                                   color: "white"
//                               }
//                           }
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
                   text: gStaGlobalSettings.getDebugIP()
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
                   text: gStaGlobalSettings.getDebugIPWifi()
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
               text: "Aktuelle Version: <a href=\"tmp\">" + gStaGlobalSettings.getCurrentVersion() + "</a>"
               onLinkActivated: {
                   var component = Qt.createComponent("../pages/newVersionInfo.qml");
                   if (component.status === Component.Ready) {
                       stackView.push(component);
                   }
               }
           }
       }
   }

   ScrollIndicator.vertical: ScrollIndicator { }

    function toolButtonClicked() {
        var saveFonts = false;

//        if (globalSettings.useReadableName !== useReadableName.checked) {
//           globalSettings.useReadableName = useReadableName.checked;
//           saveSettings = true;
//        }

        if (gStaGlobalSettings.getLoadGameInfos() !== loadGameInfo.checked) {
           gStaGlobalSettings.setLoadGameInfos(loadGameInfo.checked);
        }

        if (gStaGlobalSettings.getSaveInfosOnApp() !== saveInfosOnApp.checked) {
           gStaGlobalSettings.setSaveInfosOnApp(saveInfosOnApp.checked);
        }

        if (gStaGlobalSettings.getUseVersionPopup() !== usePopupForVersionInfo.checked) {
           gStaGlobalSettings.setUseVersionPopup(usePopupForVersionInfo.checked);
        }

        if (gStaGlobalSettings.getChangeDefaultFont() !== cbfontFamilies.currentText ) {
            gStaGlobalSettings.setChangeDefaultFont(cbfontFamilies.currentText);
            saveFonts = true;
        }

        if (gStaGlobalSettings.getDebugIP() !== txtOtherIPAddr.text) {
           gStaGlobalSettings.setDebugIP(txtOtherIPAddr.text);
        }

        if (gStaGlobalSettings.getDebugIPWifi() !== txtOtherIPAddrWifi.text) {
           gStaGlobalSettings.setDebugIPWifi(txtOtherIPAddrWifi.text);
        }

        if (notifyNewAppVersion !== gStaGlobalSettings.isNotificationNewAppVersionEnabled()) {
            gStaGlobalSettings.setNotificationNewAppVersionEnabled(notifyNewAppVersion);
        }
        if (notifyNewMeetInfo !== gStaGlobalSettings.isNotificationNewMeetingEnabled()){
            gStaGlobalSettings.setNotificationNewMeetingEnabled(notifyNewMeetInfo);
        }
        if (notifyChangeMeetInfo !== gStaGlobalSettings.isNotificationChangedMeetingEnabled()){
            gStaGlobalSettings.setNotificationChangedMeetingEnabled(notifyChangeMeetInfo);
        }
        if (notifyFreeTicket !== gStaGlobalSettings.isNotificationNewFreeTicketEnabled()){
            gStaGlobalSettings.setNotificationNewFreeTicketEnabled(notifyFreeTicket);
        }
        if (notifyAwayAccept !== gStaGlobalSettings.isNotificationNewAwayAcceptEnabled()){
            gStaGlobalSettings.setNotificationNewAwayAcceptEnabled(notifyAwayAccept);
        }
        if (notifyFanclubNews !== gStaGlobalSettings.isNotificationFanclubNewsEnabled()) {
            gStaGlobalSettings.setNotificationFanclubNewsEnabled(notifyFanclubNews);
        }

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

        cbfontFamilies.currentIndex = gStaGlobalSettings.getCurrentFontIndex()

        notifyNewAppVersion = gStaGlobalSettings.isNotificationNewAppVersionEnabled();
        notifyNewMeetInfo = gStaGlobalSettings.isNotificationNewMeetingEnabled();
        notifyChangeMeetInfo = gStaGlobalSettings.isNotificationChangedMeetingEnabled()
        notifyFreeTicket = gStaGlobalSettings.isNotificationNewFreeTicketEnabled();
        notifyAwayAccept = gStaGlobalSettings.isNotificationNewAwayAcceptEnabled();
        notifyFanclubNews = gStaGlobalSettings.isNotificationFanclubNewsEnabled();

       isStartupDone = true;
   }

   property var notifyNewAppVersion;
   property var notifyNewMeetInfo;
   property var notifyChangeMeetInfo;
   property var notifyFreeTicket;
   property var notifyAwayAccept;
   property var notifyFanclubNews;
   property var notifyDialog;
   function acceptedNotificationDialog() {
       notifyNewAppVersion = notifyDialog.enableNewAppVersion;
       notifyNewMeetInfo = notifyDialog.enableMeetingAdded;
       notifyChangeMeetInfo = notifyDialog.enableMeetingChanged;
       notifyFreeTicket = notifyDialog.enableNewFreeTicket;
       notifyAwayAccept = notifyDialog.enableNewAwayAccept;
       notifyFanclubNews = notifyDialog.enableFanclubNews
       valueWasEditedEnableSave();
   }

   function notifyUserIntConnectionFinished(result, msg) {}
}
