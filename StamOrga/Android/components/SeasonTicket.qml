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

Rectangle {
    id: ticketRectangleItem
    property var m_SeasonTicketItem

    signal clickedSeasonTicket(var sender)

    width: parent.width
    height: childrenRect.height * 1.1
    property color gradColorStart: "#501050"
    property color gradColorStop: "#905090"
    gradient: Gradient {
        GradientStop {
            position: 0
            color: gradColorStart
        }

        GradientStop {
            position: 0.5
            color: gradColorStop
        }
    }
    radius: 5
    border.color: "grey"
    border.width: 2

    ColumnLayout {
        id: columnLayoutTicketItem
        width: ticketRectangleItem.width
        spacing: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                ticketRectangleItem.clickedSeasonTicket(m_SeasonTicketItem);
            }
        }
        RowLayout {
            id: layoutNameOfTicket

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayoutTicketItem.width
            Layout.fillWidth: true

            Text {
                id: labelTicketItemHead
                text: qsTr("Karte:")
                leftPadding: 7
                topPadding: 5
                color: "white"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelTicketItem
                text: qsTr("")
                leftPadding: labelInfoDiscountHead.width - labelTicketItemHead.width + 10
                topPadding: 5
                color: "white"
                font.pixelSize: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }


        }

        RowLayout {
            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayoutTicketItem.width
            Layout.fillWidth: true

            Text {
                id: labelInfoDiscountHead
                text: qsTr("Ermäßigung: ")
                leftPadding: 7
                color: "white"
                font.pixelSize: 12
                visible: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelInfoDiscount
                text: qsTr("")
    //            rightPadding: layoutNameOfTicket.width / 10
    //            topPadding: 5
                leftPadding: 10
                visible: true
                color: "white"
                font.pixelSize: 12
                Layout.fillWidth: true
    //            Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }
        }


        RowLayout {
            id: layoutPlaceOfTicket

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayoutTicketItem.width
            Layout.fillWidth: true

            Text {
                id: labelTextWhereIsTicket
                text: qsTr("Ort:")
                leftPadding: 7
                bottomPadding: 1
                color: "white"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Text {
                id: labelInfoWhere
                text: qsTr("")
                bottomPadding: 1
                color: "white"
                font.pixelSize: 12
                leftPadding: labelInfoDiscountHead.width - labelTextWhereIsTicket.width + 10
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }
        }
    }

    function showTicketInfo(index) {
        var seasonTicketItem = globalUserData.getSeasonTicketFromArrayIndex(index)

        if (seasonTicketItem !== null) {
            m_SeasonTicketItem = seasonTicketItem
            labelTicketItem.text = m_SeasonTicketItem.name
            labelInfoWhere.text = m_SeasonTicketItem.place;
            if (m_SeasonTicketItem.discount === 0)
                labelInfoDiscount.text = "Nein";
            else
                labelInfoDiscount.text = "Ja";
            if (m_SeasonTicketItem.isTicketYourOwn())
                ticketRectangleItem.border.color = "#EF9A9A"


        }
    }
}
