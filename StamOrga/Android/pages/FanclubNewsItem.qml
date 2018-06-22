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

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2

import com.watermax.demo 1.0

import "../components" as MyComponents

Item {
    property UserInterface userIntCurrentNews
    property var newsDataItem

    Pane {
        id: mainPaneCurrentNews
        width: parent.width
        height: parent.height


        ColumnLayout {
            id: mainColumnLayoutCurrentNews
            width: parent.width
            height: parent.height

            Text {
                id: txtForFontFamily
                visible: false
            }

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorNews
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 10
            }

            MyComponents.EditableTextWithHint {
                id: textHeader
                hint: "Überschrift"
//                imageSource: ""
                width: parent.width
                enabled: isEditMode
                color: isEditMode ? "#FFFFFF" : "#536878";
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
                        font.family: txtForFontFamily.font
                        width: parent.width
//                        color: "#505050"
                        color: isEditMode ? "#505050" : "#B0BEC5";
                        background: Rectangle {
                            implicitWidth: parent.width
                            implicitHeight: parent.height
                            color: isEditMode ? "#FFFFFF" : "#536878";
                        }
                        leftPadding: 5
                        rightPadding: 5
                        font.pixelSize: 14
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

        if(!isEditMode) {
            isEditMode = true;
            updateHeaderFromMain("", "")
            isStartupDone = true;
            isTextChanged = false;
            return;
        }

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

        var newsIndex = 0;
        if (newsDataItem !== undefined)
            newsIndex = newsDataItem.index
        gDataNewsDataManager.startChangeNewsDataItem(newsIndex, textHeader.input, textAreaInfo.text);
//        userIntCurrentNews.startChangeFanclubNews(newsIndex, textHeader.input, textAreaInfo.text);
        busyIndicatorNews.loadingVisible = true;
        busyIndicatorNews.infoVisible = true;
        busyIndicatorNews.infoText = "Speichere News"
    }

    function startShowElements(newsItem, editMode) {

        newsDataItem = newsItem;
        isEditMode = editMode;
        if (editMode) {
            updateHeaderFromMain("Neue Nachricht", "")
            isStartupDone = true;
        } else {
            if (newsItem !== undefined) {
                gDataNewsDataManager.startGetNewsDataItem(newsItem.index);
                isStartupDone = false;
            }
            updateHeaderFromMain("Nachricht", "")
        }
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
            toastManager.show("Nachricht erfolgreich gespeichert", 2000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoVisible = false;
            isEditMode = false;
            updateHeaderFromMain("Nachricht", "images/edit.png")

            newsDataItem = gDataNewsDataManager.getCurrentEditedItem();

        } else {
            toastManager.show(userIntCurrentNews.getErrorCodeToString(result), 4000);
            busyIndicatorNews.loadingVisible = false;
            busyIndicatorNews.infoText = "Nachricht speichern hat nicht funktioniert"
        }
    }

    function notifyGetFanclubNewsItemFinished(result){

        if (result === 1) {
            if (gConUserSettings.userIsFanclubEditEnabled() ||  userIntCurrentNews.isDebuggingEnabled())
                updateHeaderFromMain("Nachricht", "images/edit.png")

            toastManager.show("Nachricht erfolgreich geladen", 2000);

            if (newsDataItem !== undefined) {
                textHeader.init(newsDataItem.header);
                textAreaInfo.text = newsDataItem.info;
            }
        } else {
                toastManager.show(userIntCurrentNews.getErrorCodeToString(result), 4000);
                busyIndicatorNews.loadingVisible = false;
                busyIndicatorNews.infoText = "Nachricht laden hat nicht funktioniert"
        }
    }

    function notifyUserIntConnectionFinished(result, msg) {}

}
