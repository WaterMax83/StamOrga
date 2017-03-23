#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QSocketNotifier>

#include "globaldata.h"

class Console : public QObject
{
    Q_OBJECT
public:
    explicit Console(GlobalData *pData, QObject *parent = 0);
    ~Console();

    void run();

signals:
    void quit();

private slots:
    void readCommand();

private:
    QSocketNotifier *m_pSNotify;

    QString m_applicationPath;

    GlobalData *m_pGlobalData;

    void printHelp();
    int ShowUDPHelp();

};

#endif // CONSOLE_H
