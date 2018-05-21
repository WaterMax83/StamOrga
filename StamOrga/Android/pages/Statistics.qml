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
import QtCharts 2.2

import com.watermax.demo 1.0

import "../components" as MyComponents
import "../controls" as MyControls

Item {
    id: tmp

    ColumnLayout {
        id: mainColumnLayoutStatistic
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop

            ComboBox {
                id: comboStatisticOverview
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 10
                font.family: txtForFontFamily.font
                model: gDataStatisticManager.getCurrentOverviewList()
//                anchors.horizontalCenter: parent.horizontalCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                onCurrentIndexChanged: {
                    if (iLoadingIndex < 0 || bComboBoxIndexEnabled === false)
                        return;

                    if(gDataStatisticManager.startLoadStatisticContent(comboStatisticOverview.currentIndex,
                                                                       comboStatisticYear.currentIndex) === 1) {
                        busyIndicatorStatistic.loadingVisible = true;
                        chartView.visible = false;
                        iLoadingIndex = 1;
                    }

                }
            }
            ComboBox {
                id: comboStatisticYear
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.topMargin: 10
                font.family: txtForFontFamily.font
                model: gDataStatisticManager.getCurrentYearList()
//                anchors.horizontalCenter: parent.horizontalCenter
//                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                onCurrentIndexChanged: {
                    if (iLoadingIndex < 0 || bComboBoxIndexEnabled === false)
                        return;

                    if(gDataStatisticManager.startLoadStatisticContent(comboStatisticOverview.currentIndex,
                                                                       comboStatisticYear.currentIndex) === 1) {
                        busyIndicatorStatistic.loadingVisible = true;
                        chartView.visible = false;
                        iLoadingIndex = 1;
                    }

                }
            }
        }

        Text {
            id: txtForFontFamily
            visible: false
        }

        MyComponents.BusyLoadingIndicator {
            id: busyIndicatorStatistic
            visible: !chartView.visible
            width: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: false
            Layout.topMargin: 5
            infoVisible: false
        }

        MyControls.Button {
            id: loadAgainButton
            visible: false
            text: "Erneut laden"
            font.family: txtForFontFamily.font
            implicitWidth: parent.width / 3 * 2
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                loadAgainButton.visible = false;
                busyIndicatorStatistic.infoVisible = false;
                if (iLoadingIndex === 0) {
                    gDataStatisticManager.startLoadStatisticOverview()
                    busyIndicatorStatistic.loadingVisible = true;
                    chartView.visible = false;
                } else {
                    if(gDataStatisticManager.startLoadStatisticContent(comboStatisticOverview.currentIndex,
                                                                       comboStatisticYear.currentIndex) === 1) {
                        busyIndicatorStatistic.loadingVisible = true;
                        chartView.visible = false;
                        iLoadingIndex = 1;
                    }
                }

            }
        }

        Flickable {
            id: statFlickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            boundsBehavior: Flickable.StopAtBounds
            contentHeight: chartView.height

            ChartView {
                id: chartView
                title: gDataStatisticManager.title
                width: parent.width
                height: Math.max(mainColumnLayoutStatistic.height - comboStatisticOverview.height,
                                 gDataStatisticManager.height * txtForFontFamily.height * 3)
                antialiasing: true

                HorizontalStackedBarSeries {
                    id: mySeries
                    axisY: BarCategoryAxis {
                        categories: gDataStatisticManager.categories
                    }
                    axisX: ValueAxis {
                        min: 0
                        max: gDataStatisticManager.maxX
                    }
                }
            }
        }
    }

    property int iLoadingIndex: -1
    property bool bComboBoxIndexEnabled: false

    function pageOpenedUpdateView() {

        gDataStatisticManager.startLoadStatisticOverview()
        busyIndicatorStatistic.loadingVisible = true;
        chartView.visible = false;
        iLoadingIndex = 0;

    }


    function notifyStatisticsCommandFinished(result) {
        busyIndicatorStatistic.loadingVisible = false;
        loadAgainButton.visible = false;

        if (result === 1) {

            if (iLoadingIndex === 0) {

                comboStatisticOverview.model = gDataStatisticManager.getCurrentOverviewList();
                comboStatisticYear.model = gDataStatisticManager.getCurrentYearList();
                if(gDataStatisticManager.startLoadStatisticContent(comboStatisticOverview.currentIndex,
                                                                   comboStatisticYear.currentIndex) === 1) {
                    busyIndicatorStatistic.loadingVisible = true;
                    chartView.visible = false;
                    iLoadingIndex = 1;
                }
                bComboBoxIndexEnabled = true;
            } else {

                mySeries.clear();

                for(var index = 0; index < gDataStatisticManager.getStatBarCount(); index++) {
                    var barInfo = gDataStatisticManager.getNextStatBar(index);
                    var bar = mySeries.append(barInfo.title, barInfo.values);
                    bar.color = barInfo.color;
                }
                chartView.visible = true;
            }

        } else {
            busyIndicatorStatistic.infoVisible = true;
            busyIndicatorStatistic.infoText = "Fehler beim Laden";
            loadAgainButton.visible = true;
            toastManager.show(userInt.getErrorCodeToString(result), 4000);
        }
    }
}
