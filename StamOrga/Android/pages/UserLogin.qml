import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

//import "../components" as MyComponents

Flickable {
    id: flickableUser
    contentHeight: pane.height

    property UserInterface userIntUser

    Pane {
        id: pane
        width: parent.width

        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

        ColumnLayout {
            id: mainColumnLayout
            width: pane.width
            spacing: 25

            TextField {
                id: txtIPAddress
                text: globalUserData.ipAddr
                implicitWidth: mainColumnLayout.width / 3 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                placeholderText: qsTr("Text Field")
                visible: userIntUser.isDebuggingEnabled()
            }

//            SpinBox {
//                id: spBoxPort
//                to: 100000
//                from: 1
//                value: globalUserData.conMasterPort
//                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
//            }

            ColumnLayout {
                id: columnLayout2
                spacing: 5
                width: mainColumnLayout.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: label1
                    text: qsTr("Benutzername")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtUserName
                    text: globalUserData.userName
                    padding: 8
                    implicitWidth: mainColumnLayout.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: columnLayout3
                width: mainColumnLayout.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: label2
                    text: qsTr("Passwort")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtPassWord
                    text: globalUserData.passWord
                    implicitWidth: mainColumnLayout.width / 3 * 2
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: columnLayout
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                BusyIndicator {
                    id: busyConnectIndicator
                    opacity: 1
                    visible: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                Label {
                    id: txtInfoConnecting
                    text: qsTr("Label")
                    visible: false
                }
            }

            Button {
                id: btnSendData
                text: qsTr("Verbindung prüfen")
                implicitWidth: mainColumnLayout.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                onClicked: {
                    if (userIntUser.isDebuggingEnabled()) {
                        globalUserData.ipAddr = txtIPAddress.text
                    }
//                    globalUserData.conPort = spBoxPort.value
                    if (userIntUser.startMainConnection(txtUserName.text, txtPassWord.text) > 0) {
                        btnSendData.enabled = false
                        busyConnectIndicator.visible = true;
                        txtInfoConnecting.text = "Verbinde ..."
                        txtInfoConnecting.visible = true;
                    }
                }
            }

            ToolSeparator {
                id: toolSeparator2
                orientation: "Horizontal"
                implicitWidth: mainColumnLayout.width / 3 * 1
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Button {
                id: btnChangeReadableName
                text: qsTr("Öffentlicher Name")
                implicitWidth: mainColumnLayout.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                enabled: globalUserData.bIsConnected
                onClicked: {
                    txtReadableName.text = globalUserData.readableName
                    changeReadableName.open()
                }
//                onEnabledChanged: {
//                    if (btnChangeReadableName.enabled) {
//                        /* Is called before readable name is there */
//                        if (globalUserData.readableName.length == 0)
//                            btnChangeReadableName.background.color = "#FFAAAA"
//                    }
//                }
            }

            ToolSeparator {
                id: toolSeparator3
                orientation: "Horizontal"
                implicitWidth: mainColumnLayout.width / 3 * 1
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Button {
                id: btnChangePassWord
                text: qsTr("Password ändern")
                implicitWidth: mainColumnLayout.width / 4 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                transformOrigin: Item.Center
                enabled: globalUserData.bIsConnected
                onClicked: {
                    txtnewPassWord.text = ""
                    txtnewPassWordReplay.text = ""
                    changePassWordDialog.open()
                }
            }

        }
    }


    function notifyUserIntConnectionFinished(result) {
        btnSendData.enabled = true
        busyConnectIndicator.visible = false;
        if (result === 1) {
            btnSendData.background.color = "green"
            txtInfoConnecting.text = "Verbindung erfolgreich"
        }
        else {
            btnSendData.background.color = "red"
            txtInfoConnecting.text = userIntUser.getErrorCodeToString(result);
        }
    }


    function notifyUserIntVersionRequestFinished(result, msg) {
        if (result === 5) {
            var component = Qt.createComponent("../components/VersionDialog.qml");
            if (component.status === Component.Ready) {
                var dialog = component.createObject(parent,{popupType: 1});
                dialog.versionText = msg;
                dialog.parentHeight = flickableUser.height
                dialog.parentWidth = flickableUser.width
                dialog.open();
            }
        }
    }

    function notifyUserIntUpdatePasswordFinished(result) {
        busyConnectIndicator.visible = false;
        if (result === 1)
            txtInfoConnecting.text = "Passwort erfolgreich geändert"
        else
            txtInfoConnecting.text = "Fehler beim Passwort ändern"
    }


    function notifyUserIntUpdateReadableNameFinished(result) {
        busyConnectIndicator.visible = false;
        if (result === 1)
            txtInfoConnecting.text = "Name erfolgreich geändert"
        else
            txtInfoConnecting.text = "Fehler beim Namen ändern"
    }

    function pageOpenedUpdateView() {

    }

    Dialog {
        id: changePassWordDialog
        x: Math.round((flickableUser.width - width) / 2)
        y: Math.round(flickableUser.height / 6)
        width: Math.round(Math.min(flickableUser.width, flickableUser.height) / 3 * 2)
        modal: true
        focus: true
        title: "Password ändern"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
//            settings.style = styleBox.displayText
            labelPasswordTooShort.visible = false
            labelPasswordDiffer.visible = false
            if (txtnewPassWord.text.length < 6) {
                labelPasswordTooShort.visible = true
                changePassWordDialog.open()
            } else if (txtnewPassWord.text == txtnewPassWordReplay.text) {
                userIntUser.startUpdateUserPassword(txtnewPassWord.text)
                busyConnectIndicator.visible = true;
                txtInfoConnecting.visible = true;
                txtInfoConnecting.text = "Ändere Passwort"
                changePassWordDialog.close()
            } else {
                labelPasswordDiffer.visible = true
                changePassWordDialog.open()
            }
        }
        onRejected: {
            changePassWordDialog.close()
            labelPasswordTooShort.visible = false
            labelPasswordDiffer.visible = false
        }

        contentItem: ColumnLayout {
            id: changePasswordColumn
            width: changePassWordDialog.width
            spacing: 20

            ColumnLayout {
                id: columnLayoutChPass2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: labelChaPass1
                    text: qsTr("neues Passwort")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWord
                    text: globalUserData.passWord
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            ColumnLayout {
                id: columnLayoutChPass3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Label {
                    id: labelChaPass2
                    text: qsTr("Passwort wiederholen")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                TextField {
                    id: txtnewPassWordReplay
                    text: globalUserData.passWord
                    implicitWidth: changePasswordColumn.width / 4 * 3
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }
            }

            Label {
                id: labelPasswordDiffer
                visible: false
                text: qsTr("Passwörter stimmen nicht überein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "red"
            }
            Label {
                id: labelPasswordTooShort
                visible: false
                text: qsTr("Das Passwort muss mindestens 6 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "orange"
            }
        }
    }

    Dialog {
        id: changeReadableName
        x: Math.round((flickableUser.width - width) / 2)
        y: Math.round(flickableUser.height / 6)
        width: Math.round(Math.min(flickableUser.width, flickableUser.height) / 3 * 2)
        modal: true
        focus: true
        title: "Öffentlicher Name"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            labelNameTooShort.visible = false
            if (txtReadableName.text.length < 3) {
                labelNameTooShort.visible = true
                changeReadableName.open()
            } else {
                userIntUser.startUpdateReadableName(txtReadableName.text)
                busyConnectIndicator.visible = true;
                txtInfoConnecting.visible = true;
                txtInfoConnecting.text = "Ändere Öffentlichen Namen"
                changeReadableName.close()
            }
        }
        onRejected: {
            changeReadableName.close()
            labelNameTooShort.visible = false
        }

        contentItem: ColumnLayout {
            id: changeReadableNameColumn
            width: changePassWordDialog.width
            spacing: 20

            TextField {
                id: txtReadableName
                implicitWidth: changeReadableNameColumn.width / 4 * 3
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Label {
                id: labelNameTooShort
                visible: false
                text: qsTr("Der Name muss mindestens 3 Zeichen lang sein")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                wrapMode: Text.WordWrap
                Layout.maximumWidth: parent.width
                color: "orange"
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}



