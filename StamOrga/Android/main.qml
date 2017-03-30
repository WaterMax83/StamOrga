import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

ApplicationWindow {
    visible: true
    width: 540
    height: 960
    title: qsTr("StamOrga")

//    SwipeView {
//        id: swipeView
//        anchors.fill: parent
//        currentIndex: tabBar.currentIndex

    header: ToolBar {
//        Material.foreground: "white"
        height: 75

        RowLayout {
            spacing: 20
            anchors.fill: parent

//            ToolButton {
//                contentItem: Image {
//                    fillMode: Image.Pad
//                    horizontalAlignment: Image.AlignHCenter
//                    verticalAlignment: Image.AlignVCenter
//                    source: "qrc:/images/drawer.png"
//                }
//                onClicked: drawer.open()
//            }

            Label {
                id: titleLabel
                text: "StamOrga"
                font.pixelSize: 25
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

//            ToolButton {
//                contentItem: Image {
//                    fillMode: Image.Pad
//                    horizontalAlignment: Image.AlignHCenter
//                    verticalAlignment: Image.AlignVCenter
//                    source: "qrc:/images/menu.png"
//                }
//                onClicked: optionsMenu.open()

//                Menu {
//                    id: optionsMenu
//                    x: parent.width - width
//                    transformOrigin: Menu.TopRight

//                    MenuItem {
//                        text: "Settings"
//                        onTriggered: settingsPopup.open()
//                    }
//                    MenuItem {
//                        text: "About"
//                        onTriggered: aboutDialog.open()
//                    }
//                }
//            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Pane {
            id: pane

//            background: Color.blue
//            background.color: "red"
            Rectangle {
                id: arrow
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color: "red"
                width: 200
                height: 250
            }
        }
//        UserLogin {
//            anchors.fill: parent
//        }

    }

    function openUserLogin() {
        stackView.replace("qrc:/UserLogin.qml")
    }




//

//        Page {
//            Label {
//                text: qsTr("Second page")
//                anchors.centerIn: parent
//            }
//        }
//    }
}

//    footer: TabBar {
//        id: tabBar
//        currentIndex: swipeView.currentIndex
//        TabButton {
//            text: qsTr("First")
//        }
//        TabButton {
//            text: qsTr("Second")
//        }
//    }
