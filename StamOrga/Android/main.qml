import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0



ApplicationWindow {
    visible: true
    width: 540
    height: 960
    title: qsTr("Hello World")

//    SwipeView {
//        id: swipeView
//        anchors.fill: parent
//        currentIndex: tabBar.currentIndex

    StackView {
        id: stackView
        anchors.fill: parent
        Page1 {
            anchors.fill: parent
        }

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
}
