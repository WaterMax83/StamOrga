#include "globalfunctions.h"


QString getUserHomePath()
{
    return QDir::homePath();
}

QString getUserHomeConfigPath()
{
    QString home = getUserHomePath();
    home.append("/.config/StFaeKSC");
    return home;
}

bool checkFilePathExistAndCreate(const QString &path)
{
    QFileInfo checkFileInfo(path);
    if (!checkFileInfo.exists())
    {
        /* File is not present, create it */
        qInfo().noquote() << QString("File %1 not found, maybe first run, try to create it: ").arg(path);
        if (!checkFileInfo.dir().exists() && !checkFileInfo.dir().mkpath(checkFileInfo.dir().absolutePath()))
        {
            qCritical() << "Error creating directory\n";
            return false;
        }
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
        {
            qCritical() << "Error creating file\n";
            return false;
        }
        qInfo().noquote() << "SUCCESS";
    }
    return true;
}

QString getErrorCodeString(qint32 code)
{
    switch(code) {
    case ERROR_CODE_SUCCESS:
        return QString("Success: %1").arg(code);
    case ERROR_CODE_NO_ERROR:
        return QString("No Error: %1").arg(code);
    case ERROR_CODE_COMMON:
        return QString("Common unkown error: %1").arg(code);
    case ERROR_CODE_WRONG_SIZE:
        return QString("Wrong data size: %1").arg(code);
    case ERROOR_CODE_ERR_SEND:
        return QString("Error sending data: %1").arg(code);
    case ERROR_CODE_NO_ANSWER:
        return QString("Did not get an answer: %1").arg(code);
    case ERROR_CODE_NO_USER:
        return QString("User not known: %1").arg(code);
    case ERROR_CODE_WRONG_PASSWORD:
        return QString("Password invalid: %1").arg(code);
    default:
        return QString("Unkown error: %1").arg(code);
    }
}

#define DEFAULT_PADDING_SIZE    4

inline uint CalculatePaddingSize(uint uLength)
{
    int nRem = uLength % DEFAULT_PADDING_SIZE;
    if (nRem > 0)
        return DEFAULT_PADDING_SIZE - nRem;
    else
        return 0;
}
