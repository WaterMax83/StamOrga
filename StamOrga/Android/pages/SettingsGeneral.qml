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
    id: flickableSettingsGeneral
    clip: true
    contentHeight: mainPaneSettingsGeneral.height

    flickableDirection: Flickable.VerticalFlick
    boundsBehavior: Flickable.StopAtBounds


    Pane {
        id: mainPaneSettingsGeneral
        width: parent.width

        ColumnLayout {
            id: mainColumnLayoutSettingsGeneral
            width: parent.width

            RowLayout {
                Layout.preferredWidth: parent.width
                Layout.fillWidth: true

                Text {
                    text: qsTr("Aktualisiere Spiele automatisch:")
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 14
                    color: "white"
                }
                CheckBox {
                    checked: gStaGlobalSettings.getLoadGameInfos()
                    onCheckedChanged:{
                        if (isGeneralStartupDone)
                            gStaGlobalSettings.setLoadGameInfos(checked);
                    }
                }
            }

            RowLayout {
                Layout.preferredWidth: parent.width
                Layout.fillWidth: true

                Text {
                    text: qsTr("Speichere geladene Daten:")
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 14
                    color: "white"
                }
                CheckBox {
                    checked: gStaGlobalSettings.getSaveInfosOnApp()
                    onCheckedChanged:{
                        if (isGeneralStartupDone)
                            gStaGlobalSettings.setSaveInfosOnApp(checked);
                    }
                }
            }
            RowLayout {
                Layout.preferredWidth: parent.width
                Layout.fillWidth: true

                Text {
                    text: qsTr("Nutze Popup als Versionsinfo:")
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 14
                    color: "white"
                }
                CheckBox {
                    checked: gStaGlobalSettings.getUseVersionPopup()
                    onCheckedChanged:{
                        if (isGeneralStartupDone)
                            gStaGlobalSettings.setUseVersionPopup(checked);
                    }
                }
            }
            RowLayout {
                Layout.preferredWidth: parent.width
                Layout.fillWidth: true

                Text {
                    text: qsTr("SSL Verschlüsselung:")
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    font.pixelSize: 14
                    color: gStaGlobalSettings.getCanUseSSL() ? "white" : "grey"
                }
                CheckBox {
                    id: useSSLForConnection
                    enabled: gStaGlobalSettings.getCanUseSSL();
                    checked: gStaGlobalSettings.getUseSSLSettings();
                    onCheckedChanged:{
                        if (isGeneralStartupDone) {
                            gStaGlobalSettings.setUseSSL(checked);
                            if (!isSSLRestartShown)
                                showRestartInfo();
                            isSSLRestartShown = true;
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
                    onCurrentTextChanged: {
                        if (isGeneralStartupDone) {
                            gStaGlobalSettings.setChangeDefaultFont(currentText);
                            if (!isFontRestartShown)
                                showRestartInfo();
                            isFontRestartShown = true;
                        }
                    }
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
                    text: gStaGlobalSettings.getDebugIP()
                    implicitWidth: parent.width
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: true
                    onTextChanged: {
                        if (isGeneralStartupDone)
                            gStaGlobalSettings.setDebugIP(text);
                    }
                }
            }
            //            RowLayout {
            //                spacing: 5
            //                width: parent.width
            //                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            //                visible: userInt.isDebuggingEnabled() && userInt.isDeviceMobile()

            //                Text {
            //                    text: qsTr("Alternative IP Wlan")
            //                    font.pixelSize: 14
            //                    color: "white"
            //                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            //                }

            //                TextField {
            //                    id: txtOtherIPAddrWifi
            //                    text: gStaGlobalSettings.getDebugIPWifi()
            //                    implicitWidth: parent.width
            //                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            //                    Layout.fillWidth: true
            //                    onTextChanged: {
            //                        if (isGeneralStartupDone)
            //                            gStaGlobalSettings.setDebugIPWifi(txtOtherIPAddrWifi.text);
            //                    }
            //                }
            //            }
        }
    }

    function showRestartInfo()
    {
        var component = Qt.createComponent("../components/AcceptDialog.qml");
        if (component.status === Component.Ready) {
            var dialog = component.createObject(mainPaneSettingsGeneral,{popupType: 1});
            dialog.headerText = "Information";
            dialog.parentHeight = mainWindow.height
            dialog.parentWidth = mainWindow.width
            dialog.textToAccept = "Zum Aktualisieren muss die App neu gestarted werden";
            dialog.showCancelButton = false
            dialog.open();
        }
    }

    function toolButtonClicked() {    }

    property bool isGeneralStartupDone: false
    property bool isSSLRestartShown : false
    property bool isFontRestartShown : false

    function pageOpenedUpdateView() {

        if (isGeneralStartupDone)
            return;

        cbfontFamilies.currentIndex = gStaGlobalSettings.getCurrentFontIndex()

        isGeneralStartupDone = true
    }

}
