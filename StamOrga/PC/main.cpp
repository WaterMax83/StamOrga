/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include <QApplication>

#include "../cstaglobalmanager.h"

cStaGlobalManager* g_GlobalManager;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    cStaGlobalManager globalManager;
    g_GlobalManager = &globalManager;
    globalManager.initialize();

    MainWindow w;
    w.show();

    return a.exec();
}
