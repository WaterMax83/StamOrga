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
                text: "StamOrga"
                font.pixelSize: 25
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
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
        titleLabel.text = "Benutzer"
        stackView.push("qrc:/pages/UserLogin.qml")
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
