#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QString>

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


extern bool CheckFilePathExistAndCreate(const QString &path);


#define ERROR_CODE_SUCCESS          1
#define ERROR_CODE_NO_ERROR         0
#define ERROR_CODE_COMMON           -1
#define ERROR_CODE_WRONG_SIZE       -2
#define ERROR_CODE_NO_USER          -20
#define ERROR_CODE_WRONG_PASSWORD   -21


extern QString getErrorCodeString(qint32 code);




#endif // GLOBALFUNCTIONS_H
