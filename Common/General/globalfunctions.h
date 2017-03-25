#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


#include <iostream>


#define CONSOLE_DEBUG(msg)      std::cout << msg.toStdString() << std::endl; qDebug().noquote() << msg;
#define CONSOLE_INFO(msg)       std::cout << msg.toStdString() << std::endl; qInfo().noquote() << msg;
#define CONSOLE_WARNING(msg)    std::cout << msg.toStdString() << std::endl; qWarning().noquote() << msg;
#define CONSOLE_CRITICAL(msg)   std::cout << msg.toStdString() << std::endl; qCritical().noquote() << msg;
#define CONSOLE_ONLY(msg)       std::cout << msg.toStdString() << std::endl


inline void SetMessagePattern()
{
    qSetMessagePattern("%{time dd.MM hh:mm:ss.zzz} %{type} - %{message}");
}


inline bool CheckFilePathExistAndCreate(const QString &path)
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




#endif // GLOBALFUNCTIONS_H
