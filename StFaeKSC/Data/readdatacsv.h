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
