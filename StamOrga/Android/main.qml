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
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source:  "images/refresh.png"
                    visible: stackView.depth > 1 ? false : true
                }
                onClicked: {

                    if (userInt.startMainConnection(globalUserData.userName, globalUserData.passWord) > 0) {
                        busyLoadingIndicator.visible = true
                        txtInfoLoading.text = "Verbinde"
                        txtInfoLoading.visible = true
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
                    stackView.push(model.source)
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement { title: "Benutzer"; source: "qrc:/pages/UserLogin.qml" }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Flickable {
            id: flickable
        //    width: parent.width
            contentHeight: mainPane.height
            Pane {
            id: mainPane
            width: parent.width

                ColumnLayout {
                    id: mainColumnLayout
                    anchors.right: parent.right
                    anchors.left: parent.left
                    width: parent.width
                    ColumnLayout {
                        id: columnLayoutBusyInfo
                        spacing: 5
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                        BusyIndicator {
                            id: busyLoadingIndicator
                            visible: false
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        }

                        Label {
                            id: txtInfoLoading
                            visible: false
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        }
                    }
                    ColumnLayout {
                        id: columnLayoutGames
                        anchors.right: parent.right
                        anchors.left: parent.left
                        width: parent.width
                        spacing: 10
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    }
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    Component {
        id: viewUserLogin

        MyPages.UserLogin {}
    }

    Component {
        id: gameView

        MyComponents.Games {}
    }


    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished : {
            if (result === 1) {
                userInt.startGettingGamesList()
                txtInfoLoading.text = "Lade Spielliste"
            } else {
                busyLoadingIndicator.visible = false
                if (result === -3)
                    txtInfoLoading.text = "Fehler: keine Verbindung"
                else
                    txtInfoLoading.text = "Benutzerdaten fehlerhaft"
            }
        }
        onNotifyGamesListFinished : {
            busyLoadingIndicator.visible = false
            if (result === 1) {
                showListedGames()
            } else {
                txtInfoLoading.text = "Fehler beim Lesen der Daten: " + result
            }
        }
    }



    function openUserLogin(open) {
        if (open === true) {
            listView.currentIndex = 0
            stackView.push(viewUserLogin);
        }
        showListedGames()
    }

    function showListedGames() {

        for (var j = columnLayoutGames.children.length; j > 0; j--) {
            columnLayoutGames.children[j-1].destroy()
        }

        txtInfoLoading.visible = true
        if (globalUserData.getGamePlayLength() > 0) {
            for (var i=0; i<globalUserData.getGamePlayLength(); i++) {
                var sprite = gameView.createObject(columnLayoutGames)
                sprite.showGamesInfo(globalUserData.getGamePlay(i))
            }
            txtInfoLoading.text = "Letzes Update am " + globalUserData.getGamePlayLastUpdate()
        } else
            txtInfoLoading.text = "Keine Daten zum anzeigen"
    }
}

