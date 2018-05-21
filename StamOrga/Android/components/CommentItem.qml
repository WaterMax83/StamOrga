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
import QtGraphicalEffects 1.0

import QtQuick.Controls.Material 2.2

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    width: parent.width
    height: columnLayoutComment.height

    ColumnLayout {
        id: columnLayoutComment
        width: parent.width
        spacing: 5
        RowLayout {
            id: rowNameAndDate
            width: parent.width
            spacing: 10

            Text {
                id: txtUser
                color: "#B0BEC5"
                font.bold: true
                font.pixelSize: 14
            }
//            Text {
//                color: "grey"
//                Text: " Mittelpunkt as UTF8"
//            }

            Text {
                id: txtTimeStamp
                color: "grey"
                Layout.fillWidth: true
                font.pixelSize: 14
            }
        }

        Text {
            id: txtComment
            color: "white"
            wrapMode: Text.WordWrap
            font.pixelSize: 14
            Layout.bottomMargin: 10
            Layout.fillWidth: true
        }

        Rectangle {
            color: Material.hintTextColor
            height: 1
            Layout.fillWidth: true
        }

    }

    function showCommentItem(commentItem) {
        if (commentItem === null)
            return;

        txtUser.text = commentItem.getUser();
        txtTimeStamp.text = commentItem.getReadableTime();
        txtComment.text = commentItem.getComment();
    }
}
