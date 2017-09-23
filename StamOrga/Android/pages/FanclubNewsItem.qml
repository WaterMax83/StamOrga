/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	StamOrga is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with StamOrga.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    property UserInterface userIntCurrentNews

    Pane {
        id: mainPaneCurrentNews
        width: parent.width
        height: parent.height


        ColumnLayout {
            id: mainColumnLayoutCurrentNews
            width: parent.width
            height: parent.height

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorNews
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
            }

            MyComponents.EditableTextWithHint {
                id: textHeader
                hint: "Überschrift"
                imageSource: ""
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                onTextInputChanged: checkNewTextInput();
            }

            Item {
                width: parent.width
                Layout.fillHeight: true

                Flickable {
                    anchors.fill: parent
                    flickableDirection: Flickable.VerticalFlick

                    TextArea.flickable: TextArea {
                        id: textAreaInfo
                        width: parent.width
                        color: "#505050"
                        background: Rectangle {
                            implicitWidth: parent.width
                            implicitHeight: parent.height
                            color: isEditMode ? "#FFFFFF" : "#AAAAAA";
                        }
                        leftPadding: 5
                        rightPadding: 5
                        font.pixelSize: 16
                        wrapMode: TextEdit.Wrap
                        textFormat: Qt.RichText
                        focus: true
                        enabled: isEditMode
                        selectByMouse: true
                        persistentSelection: true
                        onTextChanged: checkNewTextInput();
                    }

                    ScrollBar.vertical: ScrollBar {
                        id: currentScrollBar
                        contentItem: Rectangle {
                                implicitWidth: 3
                                implicitHeight: 100
                                radius: width / 2
                                color: currentScrollBar.pressed ? "#0080FF" : "#58ACFA"
                            }
                    }
                }
            }

            ToolBar {
                visible: isEditMode && textAreaInfo.activeFocus
                width: parent.width
                Row {
                    id: toolRow

                    ToolButton {
                        id: cutButton
                        // Don't want to close the virtual keyboard when this is clicked.
                        focusPolicy: Qt.NoFocus
                        checkable: true
                        enabled: textAreaInfo.selectedText
                        onClicked: textAreaInfo.cut()
                        contentItem: Image {
                            fillMode: Image.Pad
                            horizontalAlignment: Image.AlignHCenter
                            verticalAlignment: Image.AlignVCenter
                            source: "../images/cut.png"
                        }
                    }
                    ToolButton {
                        id: copyButton
                        // Don't want to close the virtual keyboard when this is clicked.
                        focusPolicy: Qt.NoFocus
                        checkable: true
                        enabled: textAreaInfo.selectedText
                        onClicked: textAreaInfo.copy()
                        contentItem: Image {
                            fillMode: Image.Pad
                            horizontalAlignment: Image.AlignHCenter
                            verticalAlignment: Image.AlignVCenter
                            source: "../images/copy.png"
                        }
                    }
                    ToolButton {
                        id: pasteButton
                        // Don't want to close the virtual keyboard when this is clicked.
                        focusPolicy: Qt.NoFocus
                        checkable: true
                        enabled: textAreaInfo.canPaste
                        onClicked: textAreaInfo.paste()
                        contentItem: Image {
                            fillMode: Image.Pad
                            horizontalAlignment: Image.AlignHCenter
                            verticalAlignment: Image.AlignVCenter
                            source: "../images/paste.png"
                        }
                    }
                }
            }
        }
    }

    function pageOpenedUpdateView() {}

    function toolButtonClicked() {
        if (textHeader.input.length < 1) {
            var component = Qt.createComponent("../components/AcceptDialog.qml");
            if (component.status === Component.Ready) {
                var dialog = component.createObject(mainPaneCurrentGame,{popupType: 1});
                dialog.headerText = "Information";
                dialog.parentHeight = mainPaneCurrentNews.height
                dialog.parentWidth = mainPaneCurrentNews.width
                dialog.textToAccept = "Die Überschrift darf nicht leer sein";
                dialog.showCancelButton = false
                dialog.open();
            }
            return;
        }

        userIntCurrentNews.startChangeFanclubNews(0, textHeader.input, textAreaInfo.text);
        busyIndicatorNews.loadingVisible = true;
        busyIndicatorNews.infoVisible = true;
        busyIndicatorNews.infoText = "Speichere News"
    }

    function startShowElements(editMode) {

        isEditMode = editMode;
        if (editMode)
            updateHeaderFromMain("Neue Nachricht", "")
        else {
            if (globalUserData.userIsFanclubEditEnabled() ||  userIntCurrentNews.isDebuggingEnabled())
                updateHeaderFromMain("Nachricht", "images/edit.png")
            else
                updateHeaderFromMain("Nachricht", "")
        }
        isStartupDone = true;
    }

    property bool isEditMode : false;
    property bool isTextChanged : false;
    property bool isStartupDone : false;
    function checkNewTextInput() {

        if (!isStartupDone)
            return;
        if (isTextChanged)
            return;

        isTextChanged = true;
        updateHeaderFromMain("", "images/save.png")
    }

    function notifyChangeNewsDataFinished(result) {

        if (result === 1) {
            updateHeaderFromMain("", "")
            toastManager.show("News erfolgreich gespeichert", 2000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoVisible = false;
            isEditMode = false;
            updateHeaderFromMain("Nachricht", "images/edit.png")
        } else {
            toastManager.show(userIntCurrentNews.getErrorCodeToString(result), 4000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoText = "News speichern hat nicht funktioniert"
        }
    }

    function notifyUserIntConnectionFinished(result) {}

}
