#ifndef CONNECTIONHANDLING_H
#define CONNECTIONHANDLING_H

#include <QObject>

#include "mainconnection.h"
#include "dataconnection.h"
#include "globaldata.h"
#include "../Common/General/backgroundcontroller.h"


class ConnectionHandling : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionHandling(QObject *parent = 0);

    bool StartMainConnection();

    void setGlobalData(GlobalData *pData)
    {
        if (pData != NULL)
            this->m_pGlobalData = pData;
    }

//    ConnectionInfo *GetConnectionInfo() { return &this->m_conInfo; }

signals:
    void notifyConnectionFinished(bool result);

public slots:

private slots:
    void mainConReqFin(qint32 result, const QString &msg);
    void dataConLoginFinished(qint32 result);

private:
    BackgroundController m_ctrlMainCon;
    MainConnection *m_pMainCon = NULL;

    BackgroundController m_ctrlDataCon;
    DataConnection *m_pDataCon = NULL;

    GlobalData *m_pGlobalData = NULL;
};

#endif // CONNECTIONHANDLING_H
