#ifndef LOGGING_H
#define LOGGING_H

#include <QtCore/QFile>

    void logSetLogFile(QFile *pFile);

    void logMyMessageLogginOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    int showLoggingInfo(quint16 numbOfLines);

#endif // LOGGING_H
