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
    height: mainGameLayout.implicitHeight
    Layout.minimumHeight: mainGameLayout.height

    property bool showNewDataSeperator: true

    property var m_newsDataItem

    signal clickedItem(var sender)
    signal pressAndHoldItem(var sender)

    property color gradColorStart: "#003333"
    property color gradColorStop: "#008080"
    property int imageSize : 20

    MouseArea {
        anchors.fill: parent
        onClicked: {
            clickedItem(m_newsDataItem);
        }
        onPressAndHold: {
            pressAndHoldItem(m_newsDataItem);
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
        id: mainGameLayout
        anchors.fill : parent
        spacing: 3
        Rectangle {
            color: Material.hintTextColor
            height: 1
            visible: showNewDataSeperator
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
                    id: itemNewDataInfo
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

                    Text {
                        id: labelLineNewsDataShortName
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
                        width: parent.width
                        Layout.maximumWidth: parent.width
                        Text {
                            Layout.maximumWidth: parent.width
                            anchors.left: parent.left
                            id: labelHeaderItem
                            font.bold: true
                            color: "white"
                            font.pixelSize: 14
                            wrapMode: Text.Wrap
                        }
                    }
                    RowLayout {
                        Text {
                            id: labelUserName
                            anchors.left: parent.left
                            Layout.fillWidth: true
                            color: "#B0BEC5"
                            font.pixelSize: 14
                        }

                        Text {
                            id: labelTime
                            anchors.right: parent.right
                            Layout.fillWidth: true
                            color: "grey"
                            font.pixelSize: 14
                        }
                    }
                }
            }
        }
    }

    function showNewsDataInfo(index) {
        var newsDataItem = gDataNewsDataManager.getNewsDataFromArrayIndex(index)
        if (newsDataItem !== null) {
            m_newsDataItem = newsDataItem
            labelHeaderItem.text = newsDataItem.header
            labelUserName.text = "von " + newsDataItem.user;
            labelTime.text = newsDataItem.timestampReadableLine();

            var eventCount = newsDataItem.event;
            if (eventCount > 0) {
                eventIndicator.disableVisibility = false;
                eventIndicator.eventCount = eventCount;
            } else
                eventIndicator.disableVisibility = true;

            labelLineNewsDataShortName.text = newsDataItem.getHeaderShortString();
        }
    }
}
