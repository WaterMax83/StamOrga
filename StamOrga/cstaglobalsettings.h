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

#ifndef CSTAGLOBALSETTINGS_H
#define CSTAGLOBALSETTINGS_H

#include <QObject>

#include "../Common/General/cgendisposer.h"
#include "../Common/Network/messageprotocol.h"

class cStaGlobalSettings : public cGenDisposer
{
    Q_OBJECT
public:
    explicit cStaGlobalSettings(QObject* parent = nullptr);

    qint32 initialize() override;

    qint32 startGettingVersionInfo();
    qint32 handleVersionResponse(MessageProtocol* msg);

    void setAlreadyConnected(const bool con);

    QString getRemoteVersion();
    QString getUpdateLink();
    QString getVersionInfo();

signals:

public slots:

private:
    QString m_versionInfo;
    QString m_remoteVersion;
    QString m_updateLink;

    bool m_bAlreadyConnected;
};

extern cStaGlobalSettings g_StaGlobalSettings;

#endif // CSTAGLOBALSETTINGS_H
