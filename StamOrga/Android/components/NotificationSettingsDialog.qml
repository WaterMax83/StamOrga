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

Dialog {
    property int parentWidth : 540
    property int parentHeight : 960

    property alias enableNewAppVersion : newAppVersion.checked
    property alias enableMeetingAdded : newMeeting.checked
    property alias enableMeetingChanged : changedMeeting.checked
    property alias enableNewFreeTicket : newFreeTicket.checked

    signal acceptedDialog()

    id:notificationSettingsDialog
    title: "Benachrichtigung bei"
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)
    modal: true
    focus: true

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        acceptedDialog( );
        notificationSettingsDialog.close();
        notificationSettingsDialog.destroy();
    }
    onRejected: {
        notificationSettingsDialog.close()
        notificationSettingsDialog.destroy();
    }

    contentItem: ColumnLayout {
        id: notificationSettingsDialogColumn
        width: notificationSettingsDialog.width
        spacing: 5

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

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
            }
        }

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

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
            }
        }

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

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
            }
        }

        RowLayout {
            Layout.preferredWidth: parent.width
            Layout.fillWidth: true

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
            }
        }


    }
}
