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
    property alias textToAccept : labelAcceptText.text
    property alias headerText: accepptTextDialog.title
    property bool showCancelButton: true

    signal acceptedDialog()

    id: accepptTextDialog
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)
//    Layout.maximumHeight: parentHeight / 4 * 3
    modal: true
    focus: true

    standardButtons: Dialog.Ok | (showCancelButton ? Dialog.Cancel : 0)
    onAccepted: {
        acceptedDialog();
        accepptTextDialog.close();
        accepptTextDialog.destroy();
    }
    onRejected: {
        accepptTextDialog.close()
        accepptTextDialog.destroy();
    }

    contentItem: ColumnLayout {
        id: accepptTextDialogColumn
        width: accepptTextDialog.width
        spacing: 20

        Text {
            id: labelAcceptText
            textFormat: Text.RichText
            font.pixelSize: 14
            color: "white"
            wrapMode: Text.WordWrap
            Layout.maximumWidth: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }
}
