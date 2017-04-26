/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/



import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

import "pages" as MyPages
import "components" as MyComponents

ApplicationWindow {
    id: window
    visible: true
    width: userInt.isDeviceMobile() ? 540 : 360
    height: userInt.isDeviceMobile() ? 960 : 600
    title: qsTr("StamOrga")

    onClosing: {
        if( userInt.isDeviceMobile() && stackView.depth > 1){
            close.accepted = false
            stackView.pop();
            listView.currentIndex = -1
        }else{
            return;
        }
    }

    Shortcut {
        sequence: "Esc"
        enabled: stackView.depth > 1
        onActivated: {
            stackView.pop()
            listView.currentIndex = -1
        }
    }

    header: ToolBar {
        height: 50

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: stackView.depth > 1 ? "images/back.png" : "images/drawer.png"
                }
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                        listView.currentIndex = -1
                    } else {
                        drawer.open()
                    }
                }
            }

            Label {
                id: titleLabel
                text: listView.currentItem ? listView.currentItem.text : "StamOrga"
                font.pixelSize: 25
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                contentItem: Image {
                    id: imageToolButton
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source:  "images/refresh.png"
//                    visible: stackView.depth > 1 ? false : true
                }
                onClicked: {
                    if (imageToolButton.visible === true){
                        stackView.currentItem.toolButtonClicked()

                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        dragMargin: stackView.depth > 1 ? 0 : undefined

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    stackView.push(model.element)
                    if (model.imgsource !== "") {
                        imageToolButton.visible = true
                        imageToolButton.source = model.imgsource
                    } else {
                        imageToolButton.visible = false
                    }

                    drawer.close()
                }
            }

            model: ListModel {

                Component.onCompleted: {
                    append({title: "Benutzer", element : viewUserLogin, imgsource : ""});
                    append({title: "Dauerkarten", element : viewSeasonTickets, imgsource : "images/refresh.png"})
                    if (userInt.isDebuggingEnabled() && userInt.isDeviceMobile())
                        append({title: "Logging", element : viewLoggingPage, imgsource : ""});
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: viewGames

        onCurrentItemChanged: {
            if (stackView.depth === 1) {
                imageToolButton.visible = true
                imageToolButton.source = "images/refresh.png"
            }
            stackView.currentItem.pageOpenedUpdateView()
        }
    }

    Component {
        id: viewGames
        MyPages.GamesPage {
            userIntGames: userInt
        }
    }

    Component {
        id: viewUserLogin
        MyPages.UserLogin {
            userIntUser: userInt
        }
    }

    Component {
        id: viewSeasonTickets
        MyPages.SeasonTickets {
            userIntTicket: userInt
        }
    }

    Component {
        id: viewLoggingPage
        MyPages.LogginPage {}
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished : {
             stackView.currentItem.notifyUserIntConnectionFinished(result);
        }
        onNotifyVersionRequestFinished : {
            stackView.currentItem.notifyUserIntVersionRequestFinished(result, msg);
        }
        onNotifyUpdatePasswordRequestFinished: {
            stackView.currentItem.notifyUserIntUpdatePasswordFinished(result);
        }
        onNotifyUpdateReadableNameRequest: {
            stackView.currentItem.notifyUserIntUpdateReadableNameFinished(result);
        }
        onNotifyGamesListFinished : {
            stackView.currentItem.notifyUserIntGamesListFinished(result);
        }
        onNotifySeasonTicketAddFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketAdd(result);
        }
        onNotifySeasonTicketListFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketListFinished(result);
        }
        onNotifySeasonTicketRemoveFinished: {
            stackView.currentItem.notifyUserIntSeasonTicketRemoveFinished(result);
        }
    }



    function openUserLogin(open) {
        if (open === true) {
            listView.currentIndex = 0
            stackView.push(viewUserLogin);
        } else
            stackView.currentItem.showListedGames()
    }

}

