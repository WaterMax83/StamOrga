import QtQuick 2.7

import com.watermax.demo 1.0


UserLoginForm {
    txtUserName.text: globalUserData.userName
    txtPassWord.text: globalUserData.passWord
    txtIPAddress.text: globalUserData.ipAddr
    spBoxPort.value: globalUserData.conMasterPort

    btnSendData.onClicked: {
//        console.log("Button Pressed. Entered text: " + txtUserName.text);
        globalUserData.userName = txtUserName.text
        globalUserData.passWord = txtPassWord.text
        globalUserData.ipAddr = txtIPAddress.text
        globalUserData.conPort = spBoxPort.value
        userInt.startSendingData()
        btnSendData.enabled = false
        busyConnectIndicator.visible = true;
        txtInfoConnecting.text = "Verbinde ..."
        txtInfoConnecting.visible = true;
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished : {
            btnSendData.enabled = true
            busyConnectIndicator.visible = false;
            console.log("UserLogin return value: " + result)
            if (result === 1) {
                btnSendData.background.color = "green"
                txtInfoConnecting.text = "Verbindung erfolgreich"
            }
            else {
                btnSendData.background.color = "red"
                if (result == -3)  {  // timeout
                    txtInfoConnecting.text = "Fehler: keine Antwort"
                }
                else {
                    txtInfoConnecting.text = "Verbindungsdaten fehlerhaft"
                }
            }
        }
    }
}


