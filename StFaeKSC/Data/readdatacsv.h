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

#ifndef READDATACSV_H
#define READDATACSV_H


#include <QtCore/QString>

#include "../General/globaldata.h"


class ReadDataCSV
{
public:
    ReadDataCSV(GlobalData *pGlobalData);

    int readNewCSVData(QString path);

private:
    GlobalData  *m_pGlobalData;


    int readNewGameData(QStringList line);
    int readNewUserData(QStringList line);
};

#endif // READDATACSV_H
