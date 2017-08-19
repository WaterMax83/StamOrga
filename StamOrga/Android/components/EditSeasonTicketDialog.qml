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
    property int textMinSize : 3
    property alias editableText : txtnewSeasonTicketName.text
    property alias checkBoxState : chBoxDiscount.checked
    property alias headerText: editSeasonTicketDlg.title

    signal acceptedSeasonTicketEdit(var text, var discount)

    id: editSeasonTicketDlg
    modal: true
    focus: true
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        labelTicketNameTooShort.visible = false
        if (txtnewSeasonTicketName.text.trim().length < 4) {
            labelTicketNameTooShort.visible = true
            editSeasonTicketDlg.open()
        } else {
            acceptedSeasonTicketEdit(txtnewSeasonTicketName.text.trim(), chBoxDiscount.checked ? 1 : 0);
            editSeasonTicketDlg.close();
            editSeasonTicketDlg.destroy();
        }

    }
    onRejected: {
        editSeasonTicketDlg.close()
        labelTicketNameTooShort.visible = false
    }

    contentItem: ColumnLayout {
        id: addSeasonTicketColumn
        spacing: 20
        width: editSeasonTicketDlg.width

        ColumnLayout {
            id: columnLayoutAddTicketName
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Text {
                id: labelAddTicketName
                text: qsTr("Name")
                color: "white"
                font.pixelSize: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            TextField {
                id: txtnewSeasonTicketName
                implicitWidth: addSeasonTicketColumn.width / 4 * 3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        CheckBox {
            id: chBoxDiscount
            text: "ermäßigt"
            checked: false
        }

        Text {
            id: labelTicketNameTooShort
            visible: false
            text: qsTr("Der Name muss mindestens " + textMinSize + " Zeichen lang sein")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 10
            Layout.maximumWidth: parent.width
            color: "orange"
        }
    }
}
