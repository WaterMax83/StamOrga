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


#ifndef STATISTIC_H
#define STATISTIC_H

#include <QObject>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>

#include "userinterface.h"

QT_CHARTS_USE_NAMESPACE

class Statistic : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
public:
    explicit Statistic(QObject* parent = nullptr);

    void initialize();

    Q_INVOKABLE void setUserInterface(UserInterface* pInt);

    Q_INVOKABLE qint32 loadStatisticOverview();

    Q_INVOKABLE qint32 loadStatisticContent(qint32 index);

    Q_INVOKABLE qint32 handleStatisticResponse(QByteArray& data);

    Q_INVOKABLE QStringList getCurrentOverviewList();

    Q_INVOKABLE QString getNextBarSetTitle(const qint32 index);
    Q_INVOKABLE QList<QVariant> getNextBarSetValues(const qint32 index);

    QStringList categories() { return this->m_categories; }

signals:
    void categoriesChanged();

public slots:

private:
    UserInterface* m_userInterface;
    QStringList    m_overView;

    QStringList m_categories;

    qint32 startSendCommand(QJsonObject& rootObj);
};

#endif // STATISTIC_H
