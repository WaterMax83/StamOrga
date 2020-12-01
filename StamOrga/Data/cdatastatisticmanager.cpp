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


#include "../Common/Network/messagecommand.h"
#include "../Connection/cconmanager.h"
#include "cdatastatisticmanager.h"
#include "cstaglobalmanager.h"

#define STAT_CMD_OVERVIEW 0
#define STAT_CMD_CONTENT 1

extern cStaGlobalManager* g_GlobalManager;

cDataStatisticManager* g_DataStatisticManager;

cDataStatisticManager::cDataStatisticManager(QObject* parent)
    : cGenDisposer(parent)
{
}

qint32 cDataStatisticManager::initialize()
{
    qRegisterMetaType<StatBars*>("StatBars*");

    this->m_initialized = true;

    return ERROR_CODE_SUCCESS;
}


qint32 cDataStatisticManager::startLoadStatisticOverview()
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QJsonObject rootObj;
    rootObj.insert("cmd", "overview");
    rootObj.insert("type", "cDataStatisticManager");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STATISTIC);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = STAT_CMD_OVERVIEW;

    g_ConManager->sendNewRequest(req);

    return ERROR_CODE_SUCCESS;
}

qint32 cDataStatisticManager::handleStatisticResponse(MessageProtocol* msg)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    QByteArray      data(msg->getPointerToData());
    QJsonParseError jerror;
    QJsonObject     rootObj = QJsonDocument::fromJson(data, &jerror).object();
    if (jerror.error != QJsonParseError::NoError) {
        qWarning().noquote() << QString("Could not parse statistic answer, json parse errror: %1 - %2").arg(jerror.errorString()).arg(jerror.offset);
        return ERROR_CODE_WRONG_PARAMETER;
    }

    qint32  rCode = rootObj.value("ack").toInt(ERROR_CODE_NOT_POSSIBLE);
    QString cmd   = rootObj.value("cmd").toString("");
    if (cmd.isEmpty()) {
        qWarning().noquote() << QString("No cmd found in JSON Statistics answer");
        return ERROR_CODE_MISSING_PARAMETER;
    }

    if (rCode != ERROR_CODE_SUCCESS) {
        for (int i = 0; i < this->m_statBars.count(); i++)
            delete this->m_statBars.at(i);
        this->m_statBars.clear();

        this->m_title = "Keine Daten";
        this->m_maxX  = 1;

        emit this->propertiesChanged();
        return rCode;
    }

    if (cmd == "overview") {
        QJsonArray arr   = rootObj.value("parameter").toArray();
        QJsonArray years = rootObj.value("years").toArray();

        this->m_overView.clear();
        for (int i = 0; i < arr.count(); i++) {
            QString item = arr.at(i).toString("");
            if (item.isEmpty())
                continue;
            this->m_overView.append(item);
        }
        this->m_years.clear();
        for (int i = 0; i < years.count(); i++) {
            QString item = QString::number(years.at(i).toInt());
            if (item.isEmpty())
                continue;

            this->m_years.append(item);
        }
    } else if (cmd == "content") {

        this->m_categories.clear();
        QJsonArray cat = rootObj.value("categories").toArray();
        for (int i = 0; i < cat.count(); i++)
            this->m_categories.append(cat.at(i).toString(""));

        if (this->m_categories.size() == 0)
            this->m_categories.append("Keine Daten");

        this->m_title = rootObj.value("title").toString("NoTitle");

        this->m_maxX = rootObj.value("maxX").toInt(1);

        for (int i = 0; i < this->m_statBars.count(); i++)
            delete this->m_statBars.at(i);
        this->m_statBars.clear();
        QJsonArray bars = rootObj.value("bars").toArray();
        for (int i = 0; i < bars.count(); i++) {
            QJsonObject bar = bars.at(i).toObject();

            StatBars* pBar        = new StatBars();
            pBar->m_title         = bar.value("title").toString();
            pBar->m_color         = bar.value("color").toString();
            QJsonArray jsonValues = bar.value("values").toArray();
            for (int j = 0; j < jsonValues.count(); j++)
                pBar->m_values.append(jsonValues.at(j).toInt());

            g_GlobalManager->setQMLObjectOwnershipToCpp(pBar);
            this->m_statBars.append(pBar);
        }

        emit this->propertiesChanged();
    } else
        return ERROR_CODE_NOT_FOUND;

    return rCode;
}

qint32 cDataStatisticManager::startLoadStatisticContent(qint32 catIndex, qint32 yearIndex)
{
    if (!this->m_initialized)
        return ERROR_CODE_NOT_INITIALIZED;

    if (catIndex >= this->m_overView.count())
        return ERROR_CODE_WRONG_SIZE;

    if (yearIndex >= this->m_years.count())
        return ERROR_CODE_WRONG_SIZE;

    QJsonObject rootObj;
    rootObj.insert("cmd", "content");
    rootObj.insert("parameter", this->m_overView.at(catIndex));
    rootObj.insert("year", this->m_years.at(yearIndex).toInt());
    rootObj.insert("type", "cDataStatisticManager");

    TcpDataConRequest* req = new TcpDataConRequest(OP_CODE_CMD_REQ::REQ_CMD_STATISTIC);
    req->m_lData           = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
    req->m_subCmd          = STAT_CMD_CONTENT;

    g_ConManager->sendNewRequest(req);

    return ERROR_CODE_SUCCESS;
}

StatBars* cDataStatisticManager::getNextStatBar(const qint32 index)
{
    if (this->m_statBars.count() <= index)
        return nullptr;

    return this->m_statBars.at(index);
}

QStringList cDataStatisticManager::getCurrentYearList()
{
    if (this->m_years.size() > 0)
        return this->m_years;
    QStringList tmp;
    tmp << "KeinDaten";
    return tmp;
}
