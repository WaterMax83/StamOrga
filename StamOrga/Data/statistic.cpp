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

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "statistic.h"

Statistic::Statistic(QObject* parent)
    : QObject(parent)
{
}

void Statistic::initialize()
{
    this->m_userInterface = NULL;
}


void Statistic::setUserInterface(UserInterface* pInt)
{
    this->m_userInterface = pInt;
}

qint32 Statistic::loadStatisticOverview()
{
    if (this->m_userInterface == NULL)
        return ERROR_CODE_NOT_FOUND;

    QJsonObject rootObj;
    rootObj.insert("cmd", "overview");

    return this->startSendCommand(rootObj);
}

qint32 Statistic::handleStatisticResponse(QByteArray& data)
{
    QJsonParseError jerror;
    QJsonObject     rootObj = QJsonDocument::fromJson(data, &jerror).object();
    if (jerror.error != QJsonParseError::NoError) {
        qWarning().noquote() << QString("Could not parse statistic answer, json parse errror: %1 - %2").arg(jerror.errorString()).arg(jerror.offset);
        return ERROR_CODE_WRONG_PARAMETER;
    }

    QString cmd = rootObj.value("cmd").toString("");
    if (cmd.isEmpty()) {
        qWarning().noquote() << QString("No cmd found in JSON Statistics answer");
        return ERROR_CODE_MISSING_PARAMETER;
    }

    if (cmd == "overview") {
        QJsonArray arr = rootObj.value("parameter").toArray();

        this->m_overView.clear();
        for (int i = 0; i < arr.count(); i++) {
            QString item = arr.at(i).toString("");
            if (item.isEmpty())
                continue;
            this->m_overView.append(item);
        }
    } else if (cmd == "content") {

        this->m_categories.clear();
        QJsonArray cat = rootObj.value("categories").toArray();
        for (int i = 0; i < cat.count(); i++)
            this->m_categories.append(cat.at(i).toString(""));

        QJsonArray bars = rootObj.value("bars").toArray();

        emit this->categoriesChanged();
    } else
        return ERROR_CODE_NOT_FOUND;

    return ERROR_CODE_SUCCESS;
}

qint32 Statistic::loadStatisticContent(qint32 index)
{
    if (this->m_userInterface == NULL)
        return ERROR_CODE_NOT_FOUND;

    if (index >= this->m_overView.count())
        return ERROR_CODE_WRONG_SIZE;

    QJsonObject rootObj;
    rootObj.insert("cmd", "content");
    rootObj.insert("parameter", this->m_overView.at(index));

    return this->startSendCommand(rootObj);

    return ERROR_CODE_SUCCESS;
}

QStringList Statistic::getCurrentOverviewList()
{
    return this->m_overView;
}

QString Statistic::getNextBarSetTitle(const qint32 index)
{
    if (index > 5)
        return "";

    return QString::number(index);
}

QList<QVariant> Statistic::getNextBarSetValues(const qint32 index)
{
    QList<QVariant> values;

    for (int i = 0; i < 5; i++)
        values.append(index * i);

    return values;
}

qint32 Statistic::startSendCommand(QJsonObject& rootObj)
{
    rootObj.insert("type", "Statistic");
    QByteArray data = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    return this->m_userInterface->startStatisticsCommand(data);
}
