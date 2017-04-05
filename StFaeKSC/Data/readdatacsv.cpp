#include <QtCore/QFile>
#include <QtCore/QDateTime>


#include "readdatacsv.h"
#include "../Common/General/globalfunctions.h"

ReadDataCSV::ReadDataCSV(GlobalData *pGlobalData)
{
    this->m_pGlobalData = pGlobalData;
}


enum CSV_IMPORT_GROUP {
    NOTHING = 0,
    USER = 1,
    GAME = 2
};

int ReadDataCSV::readNewCSVData(QString path)
{
    QFile csvFile(path);

    if (!csvFile.exists()) {
        CONSOLE_CRITICAL(QString("CSV File %1 does not exist").arg(path));
        return -1;
    }

    if (!csvFile.open(QIODevice::ReadOnly)) {
        CONSOLE_CRITICAL(QString("Could not open file %1: %2").arg(path).arg(csvFile.errorString()));
        return -1;
    }

    CSV_IMPORT_GROUP actGroup = CSV_IMPORT_GROUP::NOTHING;
    while (!csvFile.atEnd()) {
        QByteArray aline = csvFile.readLine();
        QString line(aline);

        if (line.trimmed() == "GAMES")
            actGroup = CSV_IMPORT_GROUP::GAME;
        else if (line.trimmed() == "USER")
            actGroup = CSV_IMPORT_GROUP::USER;
        else {
            QStringList lline = line.trimmed().split(";");

            switch(actGroup) {
            case CSV_IMPORT_GROUP::GAME:
                this->readNewGameData(lline);
                break;

            case CSV_IMPORT_GROUP::USER:
                this->readNewUserData(lline);
                break;

            default:
                break;
            }

        }
    }
    return 0;
}

int ReadDataCSV::readNewGameData(QStringList line)
{
    bool ok;
    quint8 sIndex;
    if (line.size() > 0) {
        sIndex = line.value(0).toUShort(&ok);
        if (!ok) {
            qWarning().noquote() << QString("Could not read %1 as integer value").arg(line.value(0));
            return ERROR_CODE_COMMON;
        }
    }

    quint8 competition;
    if (line.size() > 1) {
        competition = line.value(1).toUShort(&ok);
        if (!ok) {
            qWarning().noquote() << QString("Could not read %1 as integer value").arg(line.value(1));
            return ERROR_CODE_COMMON;
        }
    }

    qint64 datetime;
    if (line.size() > 2) {
        QDateTime time = QDateTime::fromString(line.value(2), "dd.MM.yyyy hh:mm");
        datetime = time.toMSecsSinceEpoch();
    }

    QString home;
    if (line.size() > 3)
        home = line.value(3);

    QString away;
    if (line.size() > 4)
        away = line.value(4);

    QString score;
    if (line.size() > 5)
        score = line.value(5);

    this->m_pGlobalData->m_GamesList.addNewGame(home, away, datetime, sIndex, score, competition);
    this->m_pGlobalData->m_GamesList.sortGamesListByTime();

    return ERROR_CODE_SUCCESS;
}

int ReadDataCSV::readNewUserData(QStringList line)
{

    return 0;
}
