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
import QtQuick.Dialogs 1.2

FileDialog {

    signal acceptedFileDialog(string url);

    id: fileDialog
    title: "Suche ein Bild"
    nameFilters: [ "Image files (*.jpg *.png)" ]
    folder: shortcuts.pictures
    selectMultiple: false
    onAccepted: {
        console.log("You chose: " + fileDialog.fileUrls)
        acceptedFileDialog(fileDialog.fileUrl);
    }
    onRejected: {
        console.log("Canceled")
    }
//    Component.onCompleted: visible = true
}
