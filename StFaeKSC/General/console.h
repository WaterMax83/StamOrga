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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QSocketNotifier>

#include "globaldata.h"

class Console : public QObject
{
    Q_OBJECT
public:
    explicit Console(GlobalData *pData, QObject *parent = 0);
    ~Console();

    void run();

signals:
    void quit();

private slots:
    void readCommand();

private:
    QSocketNotifier *m_pSNotify;

    QString m_applicationPath;

    GlobalData *m_pGlobalData;

    void printHelp();
    int ShowUDPHelp();

};

#endif // CONSOLE_H
