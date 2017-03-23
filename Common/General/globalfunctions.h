#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


inline void SetMessagePattern()
{
    qSetMessagePattern("%{time dd.MM hh:mm:ss.zzz} %{type} - %{message}");
}

bool CheckFilePathExistAndCreate(const QString &path)
{
    QFileInfo checkFileInfo(path);
    if (!checkFileInfo.exists())
    {
        /* File is not present, create it */
        qDebug().noquote() << QString("File %1 not found, maybe first run, try to create it: ").arg(path);
        if (!checkFileInfo.dir().exists() && !checkFileInfo.dir().mkpath(checkFileInfo.dir().absolutePath()))
        {
            qDebug() << "Error creating directory\n";
            return false;
        }
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error creating file\n";
            return false;
        }
        qDebug().noquote() << "SUCCESS";
    }
    return true;
}

#endif // GLOBALFUNCTIONS_H
