#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QBasicMutex>

#include <iostream>

#include "logging.h"

static QFile            *m_logFile = NULL;


struct MyLoggingInfo {
    static QBasicMutex      m_Mutex;
};

QBasicMutex MyLoggingInfo::m_Mutex;

void logSetLogFile(QFile *pFile)
{
    m_logFile = pFile;
}

void logMyMessageLogginOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker lock(&MyLoggingInfo::m_Mutex);

    QString outPutString;
    switch (type) {
    case QtDebugMsg:
        outPutString = QString("Debug:   %1\n").arg(msg);
        break;
    case QtInfoMsg:
        outPutString = QString("Info:    %1\n").arg(msg);
        break;
    case QtWarningMsg:
        outPutString = QString("Warning: %1\n").arg(msg);
        break;
    case QtCriticalMsg:
        outPutString = QString("Error:   %1 in function\n").arg(msg).arg(context.function);
        break;
    case QtFatalMsg:
        outPutString = QString("Fatal:   %1\n").arg(msg);
        abort();
    }

    if (m_logFile != NULL) {
        QTextStream out(m_logFile);
        out << QDateTime::currentDateTime().toString("dd.MMM hh:mm:ss.zzz") << ": " << outPutString;
        if (type == QtCriticalMsg)
            std::cout << outPutString.toStdString() << std::endl;
    }
    else if (type != QtDebugMsg && type != QtInfoMsg){
        std::cout << outPutString.toStdString() << std::endl;
    }
}

int showLoggingInfo(quint16 numbOfLines)
{
    QMutexLocker lock(&MyLoggingInfo::m_Mutex);

    if (m_logFile->size() > numbOfLines * 150)
        m_logFile->seek(m_logFile->size() - (numbOfLines * 150));
    else
        m_logFile->seek(0);

    QList<QByteArray> aList;

    while(!m_logFile->atEnd()) {
        QByteArray arr = m_logFile->readLine(150);
        aList.append(arr);
    }

    int cnt = 0;
    if (aList.size() > numbOfLines)
        cnt = aList.size() - numbOfLines;

    for(; cnt < aList.size(); cnt++) {
        std::cout << aList[cnt].toStdString();
    }

    return numbOfLines;
}
