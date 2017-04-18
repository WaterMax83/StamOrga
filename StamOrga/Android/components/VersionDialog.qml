import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Dialog {
    property int parentWidth : 540
    property int parentHeight : 960
    property alias versionText: versionDialogTextUpdate.text
    id: versionDialog
    modal: true
    focus: true
    title: "Neue Version"
    x: (parentWidth - width) / 2
    y: parentHeight / 6
    width: Math.min(parentWidth, parentHeight) / 3 * 2
        contentHeight: versionColumn.height

    Column {
        id: versionColumn
        spacing: 20

        Label {
            width: versionDialog.availableWidth
            text: "Es gibt eine neue Version von StamOrga"
            wrapMode: Label.Wrap
            font.pixelSize: 12
        }

        Label {
            id: versionDialogTextUpdate
            width: versionDialog.availableWidth
            wrapMode: Label.Wrap
            font.pixelSize: 12
        }
    }
}
