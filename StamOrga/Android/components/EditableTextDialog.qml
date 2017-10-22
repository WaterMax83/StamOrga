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

import "../components" as MyComponents

MyComponents.CustomDialog {
    property int parentWidth : 540
    property int parentHeight : 960
    property int textMinSize : 3
    property alias editableText : txtEditableText.text
    property alias headerText: editableTextDialog.title

    signal acceptedTextEdit(var text)

    id: editableTextDialog
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)
    modal: true
    focus: true

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        labelTextTooShort.visible = false
        if (txtEditableText.text.trim().length < textMinSize) {
            labelTextTooShort.visible = true
            labelTextTooShort.text = "Der Text muss mindestens " + textMinSize + " Zeichen lang sein";
            editableTextDialog.open()
        } else {
            acceptedTextEdit(txtEditableText.text.trim());
            editableTextDialog.close();
            editableTextDialog.destroy();
        }
    }
    onRejected: {
        editableTextDialog.close()
        labelTextTooShort.visible = false
        editableTextDialog.destroy();
    }

    contentItem: ColumnLayout {
        id: editableTextDialogColumn
        width: editableTextDialog.width
        spacing: 20

        Text {
            id: txtForFontFamily
            visible: false
        }

        TextField {
            id: txtEditableText
            font.family: txtForFontFamily.font
            implicitWidth: editableTextDialogColumn.width / 4 * 3
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Text {
            id: labelTextTooShort
            visible: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            Layout.maximumWidth: parent.width
            color: "orange"
        }
    }
}
