#include <QString>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../Common/Network/messageprotocol.h"
#include "../../Common/General/globalfunctions.h"
#include "../connectioninfo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetMessagePattern();

    this->m_pMainCon = new ConnectionHandling();
    connect(this->m_pMainCon, &ConnectionHandling::NotifyConnectionFinished, this, &MainWindow::ConnectionFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSendData_clicked()
{
    ConnectionInfo *info = this->m_pMainCon->GetConnectionInfo();
    QMutexLocker locker(&info->m_infoMutex);

    info->SetUserName(this->ui->lEditSendUserName->text());
    info->SetHostAddress(this->ui->lEditIpAddr->text());
    info->SetPort(this->ui->spBoxPort->value());
    if (this->m_pMainCon->StartMainConnection())
    {
        this->ui->btnSendData->setEnabled(false);
    }
}

void MainWindow::ConnectionFinished()
{
    this->ui->btnSendData->setEnabled(true);
}
