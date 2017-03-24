import QtQuick 2.7

import com.watermax.demo 1.0


UserLoginForm {
    txtUserName.text: globalUserData.userName
    txtIPAddress.text: globalUserData.ipAddr
    spBoxPort.value: globalUserData.conPort

    btnSendData.onClicked: {
//        console.log("Button Pressed. Entered text: " + txtUserName.text);
        globalUserData.userName = txtUserName.text
        globalUserData.ipAddr = txtIPAddress.text
        globalUserData.conPort = spBoxPort.value
        userInt.StartSendingData()
        btnSendData.enabled = false
    }

    UserInterface {
        id: userInt
        globalData: globalUserData
        onNotifyConnectionFinished : {
            btnSendData.enabled = true
            if (result)
                btnSendData.background.color = "green"
            else
                btnSendData.background.color = "red"
        }
    }
}


