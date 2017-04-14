import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0


Item {

    Rectangle {
        x: 100
        y: 100
        width: 150
        height: 150
        color: "blue"
    }

    function toolButtonClicked() {
        console.log("Clicked the ToolButton inside SeasonTIcket")
    }
}
