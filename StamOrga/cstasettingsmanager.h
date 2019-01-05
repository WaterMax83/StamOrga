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

#ifndef CSTASETTINGSMANAGER_H
#define CSTASETTINGSMANAGER_H

#include <QObject>
#include <QtCore/QSettings>

#include "../Common/General/cgendisposer.h"

class cStaSettingsManager : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cStaSettingsManager(QObject* parent = nullptr);

    qint32 initialize() override;

    qint32 getValue(const QString& group, const QString& key, QString& value, const QString& defaultValue = "");
    qint32 setValue(const QString& group, const QString& key, const QString& value);
    qint32 getValue(const QString& group, const QString& key, const qint32 index, QString& value);
    qint32 setValue(const QString& group, const QString& key, const qint32 index, const QString& value);
    qint32 getBoolValue(const QString& group, const QString& key, bool& value, const bool defaultValue = false);
    qint32 setBoolValue(const QString& group, const QString& key, const bool value);
    qint32 getBoolValue(const QString& group, const QString& key, const qint32 index, bool& value);
    qint32 setBoolValue(const QString& group, const QString& key, const qint32 index, const bool value);
    qint32 getInt32ValueDF(const QString& group, const QString& key, qint32& value, const qint32 defaultValue = 0);
    qint32 setInt32Value(const QString& group, const QString& key, const qint32 value);
    qint32 getInt32Value(const QString& group, const QString& key, const qint32 index, qint32& value);
    qint32 setInt32Value(const QString& group, const QString& key, const qint32 index, const qint32 value);
    qint32 getInt64Value(const QString& group, const QString& key, qint64& value, const qint64 defaultValue = 0);
    qint32 setInt64Value(const QString& group, const QString& key, const qint64 value);
    qint32 getInt64Value(const QString& group, const QString& key, const qint32 index, qint64& value);
    qint32 setInt64Value(const QString& group, const QString& key, const qint32 index, const qint64 value);


    qint32 removeGroup(const QString& group);

signals:

public slots:

private:
    QSettings* m_pMainUserSettings;

    qint32 getValue(const QString& group, const QString& key, QVariant& value, const QVariant defaultValue = "");
    qint32 setValue(const QString& group, const QString& key, const QVariant value);
    qint32 getValue(const QString& group, const QString& key, const qint32 index, QVariant& value, const QVariant defaultValue = "");
    qint32 setValue(const QString& group, const QString& key, const qint32 index, const QVariant value);
};

extern cStaSettingsManager* g_StaSettingsManager;

#endif // CSTASETTINGSMANAGER_H
