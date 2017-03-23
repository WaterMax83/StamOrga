import QtQuick 2.7

import com.watermax.demo 1.0


Page1Form {
    btnSendData.onClicked: {
//        console.log("Button Pressed. Entered text: " + txtUserName.text);
        userInt.StartSendingData()
        btnSendData.enabled = false
    }

    UserInterface {
        id: userInt
        userName: txtUserName.text
        ipAddr: txtIPAddress.text
        port: spBoxPort.value
        onNotifyConnectionFinished : btnSendData.enabled = true
    }
}


