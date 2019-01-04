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
    height: mainUserInfoLayout.implicitHeight
    Layout.minimumHeight: mainUserInfoLayout.height

    property bool showTextDataSeperator: true

    property var m_userInfoItem

    signal clickedItem(var sender)
    signal pressAndHoldItem(var sender)

    property color gradColorStart: "#104060"
    property color gradColorStop: "#2070b0"
    property int imageSize : 20

    MouseArea {
        anchors.fill: parent
        onClicked: {
            clickedItem(m_userInfoItem);
        }
        onPressAndHold: {
            pressAndHoldItem(m_userInfoItem);
        }
    }

//    MyComponents.EventIndicator {
//        id: eventIndicator
//        disableVisibility: true
//        eventCount : 1
//        itemSize: imageSize * 2
//        z : 1000
//    }

    ColumnLayout {
        id: mainUserInfoLayout
        anchors.fill : parent
        spacing: 3
        Rectangle {
            color: Material.hintTextColor
            height: 1
            visible: showTextDataSeperator
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
                    id: itemUserInfo
                    width: 40
                    height: 40
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
                        id: labelLineUserInfoShortName
                        text: "U"
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
                            font.pixelSize: 16
                            wrapMode: Text.Wrap
                        }

//                        Text {
//                            text: ", "
//                            color: "#B0BEC5"
//                            font.pixelSize: 14
//                        }

//                        Text {
//                            id: labelUserType

//                            Layout.fillWidth: true
//                            color: "#B0BEC5"
//                            font.pixelSize: 14
//                        }

//                        Text {
//                            id: labelTime
//                            anchors.right: parent.right
//                            anchors.rightMargin: 5
//                            color: "grey"
//                            font.pixelSize: 13
//                        }
                    }
                    RowLayout {
                        Text {
                            id: labelSubHeader
                            anchors.left: parent.left
                            Layout.fillWidth: true
                            color: "#B0BEC5"
                            font.pixelSize: 14
                        }


                    }
                }
            }
        }
    }

    function showUserInformation(index) {

        var userInfoItem;
            userInfoItem = gDataUserManager.getUserInfoFromArrayIndex(index);
        if (userInfoItem !== null) {
            m_userInfoItem = userInfoItem
            labelHeaderItem.text = userInfoItem.readName
//            labelUserType.text = userInfoItem.userType
                labelSubHeader.text = "zuletzt online vor " + userInfoItem.onlineTime;
//                labelTime.text = textDataItem.timestampReadableLine();

            if (userInfoItem.owner) {
                itemUserInfo.border.color = "orange";
                itemUserInfo.border.width = 3;
            } else {
                itemUserInfo.border.color = "grey";
                itemUserInfo.border.width = 2;
            }

            labelLineUserInfoShortName.text = userInfoItem.getUserShortString();
        }
    }
}
