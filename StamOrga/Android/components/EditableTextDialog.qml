import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

Dialog {
    property int parentWidth : 540
    property int parentHeight : 960
    property int textMinSize : 3
    property alias editableText : txtEditableText.text
    property alias headerText: editableTextDialog.title

    signal acceptedTextEdit(var text)

    id: editableTextDialog
    x: Math.round((parentWidth - width) / 2)
    y: Math.round(parentHeight / 6)
    width: Math.round(Math.min(parentWidth, parentHeight) / 3 * 2)
    modal: true
    focus: true

    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        labelTextTooShort.visible = false
        if (txtEditableText.text.length < textMinSize) {
            labelTextTooShort.visible = true
            labelTextTooShort.text = "Der Text muss mindestens " + textMinSize + " Zeichen lang sein";
            editableTextDialog.open()
        } else {
            acceptedTextEdit(txtEditableText.text);
            editableTextDialog.close();
        }
    }
    onRejected: {
        editableTextDialog.close()
        labelTextTooShort.visible = false
    }

    contentItem: ColumnLayout {
        id: editableTextDialogColumn
        width: changePassWordDialog.width
        spacing: 20

        TextField {
            id: txtEditableText
            implicitWidth: editableTextDialogColumn.width / 4 * 3
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }

        Label {
            id: labelTextTooShort
            visible: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WordWrap
            Layout.maximumWidth: parent.width
            color: "orange"
        }
    }
}
