import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: ticketRectangleItem
    property var m_SeasonTicketItem

    width: parent.width
    height: childrenRect.height * 1.1
    property color gradColorStart: "#505010"
    property color gradColorStop: "#909050"
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
    border.width: 4
    ColumnLayout {
        id: columnLayoutTicketItem
        width: ticketRectangleItem.width
        spacing: 1
        Label {
            id: labelTicketItem
            text: qsTr("")
            leftPadding: 7
            topPadding: 5
            Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
        }

        RowLayout {
            id: layoutPlaceOfTicket

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayoutTicketItem.width
            Layout.fillWidth: true

            Label {
                id: labelTextWhereIsTicket
                text: qsTr("Befindet sich bei:")
                leftPadding: 7
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelInfoWhere
                text: qsTr("")
                rightPadding: layoutPlaceOfTicket.width / 10
                bottomPadding: 5
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }
    }

    function showTicketInfo(seasontTicketItem) {
        if (seasontTicketItem !== null) {
            m_SeasonTicketItem = seasontTicketItem
            labelTicketItem.text = seasontTicketItem.name
            labelInfoWhere.text = seasontTicketItem.place;
        }
    }
}
