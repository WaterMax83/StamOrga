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

Item {
    id: tmp
    Pane {
        id: mainPaneStatistic
        width: parent.width
        height: parent.height

        ColumnLayout {
            id: mainColumnLayoutStatistic
            anchors.fill: parent
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            spacing: 0

            ComboBox {
                id: comboStatisticOverview
                font.family: txtForFontFamily.font
                model: statistics.getCurrentOverviewList()
                anchors.horizontalCenter: parent.horizontalCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                onCurrentIndexChanged: {
                    if (iLoadingIndex < 0 || bComboBoxIndexEnabled === false)
                        return;

                    statistics.loadStatisticContent(comboStatisticOverview.currentIndex);
                    busyIndicatorStatistic.loadingVisible = true;
                    chartView.visible = false;
                    iLoadingIndex = 1;

                }
            }

            Text {
                id: txtForFontFamily
                visible: false
            }

            MyComponents.BusyLoadingIndicator {
                id: busyIndicatorStatistic
                width: parent.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillHeight: false
                Layout.topMargin: 5
                infoVisible: false
            }

            MyComponents.CustomButton {
                id: loadAgainButton
                visible: false
                text: "Erneut laden"
                font.family: txtForFontFamily.font
                implicitWidth: parent.width / 3 * 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    loadAgainButton.visible = false;
                    busyIndicatorStatistic.infoVisible = false;
                    if (iLoadingIndex === 0)
                        statistics.loadStatisticOverview();
                    else
                        statistics.loadStatisticContent(comboStatisticOverview.currentIndex);
                    busyIndicatorStatistic.loadingVisible = true;
                    chartView.visible = false;
                }
            }

            ChartView {
                id: chartView
                title: "Stacked Bar series"
//                anchors.fill: parent
                Layout.fillWidth: true
                Layout.fillHeight: true
//                legend.alignment: Qt.AlignBottom
                antialiasing: true

                HorizontalStackedBarSeries {
                    id: mySeries
                    axisY: BarCategoryAxis {
//                        categories: ["2004", "2005", "2006", "2007", "2008", "2009" ]
                        categories: statistics.categories
                    }
                    BarSet { label: "Bob"; values: [2, 2, 3, 4, 5, 6] }
                    BarSet { label: "Susan"; values: [5, 1, 2, 4, 1, 7] }
                    BarSet { label: "James"; values: [3, 5, 8, 13, 5, 8] }
                }
            }
        }
    }

    property int iLoadingIndex: -1
    property bool bComboBoxIndexEnabled: false

    function pageOpenedUpdateView() {

        statistics.setUserInterface(userInt, chartView);

        statistics.loadStatisticOverview();
        busyIndicatorStatistic.loadingVisible = true;
        chartView.visible = false;
        iLoadingIndex = 0;

//        mySeries.

    }


    function notifyStatisticsCommandFinished(result) {
        console.log("Finished command " + result + " " + iLoadingIndex);

        busyIndicatorStatistic.loadingVisible = false;

        if (result === 1) {

            if (iLoadingIndex === 0) {

                comboStatisticOverview.model = statistics.getCurrentOverviewList();
                statistics.loadStatisticContent(comboStatisticOverview.currentIndex);
                iLoadingIndex = 1;
                bComboBoxIndexEnabled = true;
            } else {

                mySeries.clear();

                var index = 0;
                while(1) {
                    var title = statistics.getNextBarSetTitle(index);
                    if (title === "")
                        break;
                    var values = statistics.getNextBarSetValues(index);
                    index++;
                    mySeries.append(title, values);
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
