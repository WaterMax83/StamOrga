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
    height: mainTicketLayout.implicitHeight
    Layout.minimumHeight: mainTicketLayout.height

    property bool showTicketSeperator: true

    property var m_SeasonTicketItem

    signal clickedSeasonTicket(var sender)

    property color gradColorStart: "#501050"
    property color gradColorStop: "#905090"
    property int imageSize : 20

    MouseArea {
        anchors.fill: parent
        onClicked: {
            clickedSeasonTicket(m_SeasonTicketItem);
        }
    }

    MyComponents.EventIndicator {
        id: eventIndicator
        disableVisibility: true
        eventCount : 1
        itemSize: imageSize * 2
        z : 1000
    }

    ColumnLayout {
        id: mainTicketLayout
        anchors.fill : parent
        spacing: 3
        Rectangle {
            color: Material.hintTextColor
            height: 1
            visible: showTicketSeperator
            Layout.fillWidth: true
        }

        RowLayout {
            width: parent.width
            Layout.minimumHeight: columnItem.height
            Layout.bottomMargin: 5

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 3
                anchors.leftMargin: 3

                Rectangle {
                    id: itemTicketInfo
                    width: 50
                    height: 50
                    radius: width / 2
                    border.color: "grey"
                    border.width: 3
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: gradColorStart
                        }

                        GradientStop {
                            position: 0.9
                            color: gradColorStop
                        }
                    }
                    Image {
                        id: itemImageDiscount
                        height: 24
                        width: 24
                        source: "../images/discount.png"
                        anchors.right : parent.right
                        anchors.top: parent.top
                        rotation: 315
                    }

                    Text {
                        id: labelLineTicketShortName
                        text: "T"
                        font.pixelSize: 22
                        font.bold: true
                        color: "#B0BEC5"
                        anchors.centerIn: parent
                    }
                }
            }

            Item {
                id: columnItem
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.minimumHeight: columnItemLayout.implicitHeight
                ColumnLayout {
                    id: columnItemLayout
                    width: parent.width
                    spacing: 5

                    RowLayout {
                        Text {
                            anchors.left: parent.left
                            id: labelTicketItem
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                        }
                    }
                    RowLayout {
                        Text {
                            id: labelInfoWhere
                            anchors.left: parent.left
                            Layout.fillWidth: true
                            font.bold: true
                            color: "#B0BEC5"
                            font.pixelSize: 14
                        }

                        Text {
                            id: labelInfoSince
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            font.bold: true
                            color: "#B0BEC5"
                            font.pixelSize: 13
                        }
                    }
                }
            }
        }
    }

    function showTicketInfo(index) {
        var seasonTicketItem = gDataTicketManager.getSeasonTicketFromArrayIndex(index)

        if (seasonTicketItem !== null) {
            m_SeasonTicketItem = seasonTicketItem
            labelTicketItem.text = m_SeasonTicketItem.name
            labelInfoWhere.text = m_SeasonTicketItem.place;
            labelInfoSince.text = "seit " + m_SeasonTicketItem.getTimeStamp();

            labelLineTicketShortName.text = m_SeasonTicketItem.getTicketShortName();
            if (m_SeasonTicketItem.discount === 0)
                itemImageDiscount.visible = false
            else
                itemImageDiscount.visible = true
            if (m_SeasonTicketItem.isTicketYourOwn()) {
                itemTicketInfo.border.color = "orange";
                itemTicketInfo.border.width = 3;
            } else {
                itemTicketInfo.border.color = "grey";
                itemTicketInfo.border.width = 2;
            }
        }
    }
}
