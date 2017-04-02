import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 540
    height: 960
    title: qsTr("StamOrga")

    onClosing: {
//        if( UserInterface.isDeviceMobile && stackView.depth > 1){
        if(stackView.depth > 1){
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

        initialItem: Pane {
            id: pane
        }
    }

    function openUserLogin() {
        listView.currentIndex = 0
        stackView.push("qrc:/pages/UserLogin.qml")
    }
}

