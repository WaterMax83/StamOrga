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

    qint32 getValue(const QString group, const QString key, QString& value);

signals:

public slots:

private:
    QSettings* m_pMainUserSettings;
};

#endif // CSTASETTINGSMANAGER_H
