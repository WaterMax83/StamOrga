import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Rectangle {
    id: ticketRectangleItem
    property var m_SeasonTicketItem

    signal clickedSeasonTicket(var sender)

    width: parent.width
    height: childrenRect.height * 1.1
    property color gradColorStart: "#501050"
    property color gradColorStop: "#905090"
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
    border.width: 2

    ColumnLayout {
        id: columnLayoutTicketItem
        width: ticketRectangleItem.width
        spacing: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                ticketRectangleItem.clickedSeasonTicket(m_SeasonTicketItem);
            }
        }
        RowLayout {
            id: layoutNameOfTicket

            anchors.right: parent.right
            anchors.left: parent.left
            Layout.preferredWidth: columnLayoutTicketItem.width
            Layout.fillWidth: true

            Label {
                id: labelTicketItem
                text: qsTr("")
                leftPadding: 7
                topPadding: 5
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelInfoDiscount
                text: qsTr("ermäßigt")
                rightPadding: layoutNameOfTicket.width / 10
                topPadding: 5
                visible: false
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
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
                bottomPadding: 1
                Layout.alignment: Qt.AlignLeft | Qt.AlignHCenter
            }

            Label {
                id: labelInfoWhere
                text: qsTr("")
                rightPadding: layoutPlaceOfTicket.width / 10
                bottomPadding: 1
                Layout.alignment: Qt.AlignRight | Qt.AlignHCenter
            }
        }
    }

    function showTicketInfo(seasonTicketItem) {
        if (seasonTicketItem !== null) {
            m_SeasonTicketItem = seasonTicketItem
            labelTicketItem.text = seasonTicketItem.name
            labelInfoWhere.text = seasonTicketItem.place;
            if (seasonTicketItem.discount > 0)
                labelInfoDiscount.visible = true;
            if (seasonTicketItem.isTicketYourOwn())
                ticketRectangleItem.border.color = "#EF9A9A"


        }
    }
}
