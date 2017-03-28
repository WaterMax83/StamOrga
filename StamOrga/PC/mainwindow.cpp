#include <QString>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../Common/Network/messageprotocol.h"
#include "../../Common/General/globalfunctions.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetMessagePattern();

    this->m_pMainCon = new ConnectionHandling();
    connect(this->m_pMainCon, &ConnectionHandling::notifyConnectionFinished, this, &MainWindow::ConnectionFinished);

    this->m_pGlobalData = new GlobalData();

    this->ui->lEditSendUserName->setText(this->m_pGlobalData->userName());
    this->ui->lEditIpAddr->setText(this->m_pGlobalData->ipAddr());
    this->ui->spBoxPort->setValue(this->m_pGlobalData->conMasterPort());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSendData_clicked()
{
    this->m_pMainCon->setGlobalData(this->m_pGlobalData);
    this->m_pGlobalData->setUserName(this->ui->lEditSendUserName->text());
    this->m_pGlobalData->setIpAddr(this->ui->lEditIpAddr->text());
    this->m_pGlobalData->setConMasterPort(this->ui->spBoxPort->value());
    if (this->m_pMainCon->StartMainConnection())
    {
        this->ui->btnSendData->setEnabled(false);
    }
}

void MainWindow::ConnectionFinished(bool result)
{
    this->ui->btnSendData->setEnabled(true);
    if (result)
        this->ui->conResult->setStyleSheet("background-color:green");
    else
        this->ui->conResult->setStyleSheet("background-color:red");
}
