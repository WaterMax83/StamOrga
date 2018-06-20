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
import "../controls" as MyControls

Item {
    id: mainItemNotificationSettings
    property alias enableNewAppVersion : newAppVersion.checked
    property alias enableMeetingAdded : newMeeting.checked
    property alias enableMeetingChanged : changedMeeting.checked
    property alias enableNewFreeTicket : newFreeTicket.checked
    property alias enableNewAwayAccept : newAwayAccept.checked
    property alias enableFanclubNews: newFanclubNews.checked
    property alias enableMeetComment: newMeetComment.checked
    property alias visibleFanclubNews: rowFanclubNews.visible

//    signal acceptedDialog()

    Pane {
        id: mainPaneNotification
        width: parent.width
        height: parent.height

        Text {
            id: txtForFontFamily
            visible: false
        }

        Item {
            width: parent.width
            anchors.top : parent.top
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5

            Flickable {
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick

                ColumnLayout {
                    id: notificationSettingsDialogColumn
                    width: parent.width
                    spacing: 5

                    RowLayout {
                        Text {
                            id: text1
                            text: qsTr("Neuer App Version:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newAppVersion
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        Text {
                            id: text2
                            text: qsTr("Neues Treffen:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newMeeting
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        Text {
                            id: text3
                            text: qsTr("Treffen geändert:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: changedMeeting
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        Text {
                            id: text4
                            text: qsTr("Dauerkarte frei:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newFreeTicket
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        Text {
                            id: text5
                            text: qsTr("Erster Auswärtsfahrer:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newAwayAccept
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        Text {
                            id: text6
                            text: qsTr("Neuer Kommentar:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newMeetComment
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                    RowLayout {
                        id: rowFanclubNews
                        Text {
                            id: text7
                            text: qsTr("Fanclub Nachrichten:")
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            font.pixelSize: 14
                            color: "white"
                        }
                        CheckBox {
                            id: newFanclubNews
                            onCheckedChanged:if (isNotifyStartupDone) valueWasEditedEnableSave();
                        }
                    }

                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }

        function toolButtonClicked() {
            saveAllSettings();
        }

//        RowLayout {
//            id: rowNotificationButtons
//            width: parent.width
//            anchors.bottom: parent.bottom

//            MyControls.Button {
//                id: acceptButton
//                text: "Bestätigen"
//                font.family: txtForFontFamily.font
//                Layout.alignment: Qt.AlignHCenter
//                Layout.fillWidth: true
//                onClicked: {
//                    acceptedDialog()
//                    stackView.pop();
//                }
//            }

//            MyControls.Button {
//                id: camcelButton
//                text: "Abbrechen"
//                font.family: txtForFontFamily.font
//                Layout.alignment: Qt.AlignHCenter
//                Layout.fillWidth: true
//                onClicked: {
//                    stackView.pop();
//                }
//            }
//        }

    }

    property bool isNotifyStartupDone: false

    function pageOpenedUpdateView() {
        isNotifyStartupDone = true
    }
}
