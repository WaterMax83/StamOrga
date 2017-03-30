#include <QtCore/QString>
#include <QMessageBox>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../Common/Network/messageprotocol.h"
#include "../../Common/General/globalfunctions.h"
//#include "../../Common/General/globalfunctions.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetMessagePattern();

    this->m_pConHandling = new ConnectionHandling();
    connect(this->m_pConHandling, &ConnectionHandling::sNotifyConnectionFinished, this, &MainWindow::connectionFinished);
    connect(this->m_pConHandling, &ConnectionHandling::sNotifyVersionRequest, this, &MainWindow::versionRequestFinished);

    this->m_pGlobalData = new GlobalData();
    this->m_pGlobalData->loadGlobalSettings();

    this->ui->lEditSendUserName->setText(this->m_pGlobalData->userName());
    this->ui->lEditTextPassword->setText(this->m_pGlobalData->passWord());
    this->ui->lEditIpAddr->setText(this->m_pGlobalData->ipAddr());
    this->ui->spBoxPort->setValue(this->m_pGlobalData->conMasterPort());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSendData_clicked()
{
    this->m_pConHandling->setGlobalData(this->m_pGlobalData);
    this->m_pGlobalData->setUserName(this->ui->lEditSendUserName->text());
    this->m_pGlobalData->setPassWord(this->ui->lEditTextPassword->text());
    this->m_pGlobalData->setIpAddr(this->ui->lEditIpAddr->text());
    this->m_pGlobalData->setConMasterPort(this->ui->spBoxPort->value());
    if (this->m_pConHandling->startMainConnection())
    {
        this->ui->btnSendData->setEnabled(false);
    }
}

void MainWindow::connectionFinished(qint32 result)
{
    if (result >= ERROR_CODE_NO_ERROR) {
        this->ui->conResult->setStyleSheet("background-color:green");
        if (!this->m_pConHandling->startGettingInfo())
            this->ui->btnSendData->setEnabled(true);
    }
    else {
        this->ui->conResult->setStyleSheet("background-color:red");
        this->ui->btnSendData->setEnabled(true);
    }
}

void MainWindow::versionRequestFinished(qint32 result, QString msg)
{
    this->ui->btnSendData->setEnabled(true);

    if (result == ERROR_CODE_NEW_VERSION) {
        QMessageBox msgBox;
        msgBox.setText(msg);
        msgBox.exec();
    } else {
        qDebug() << QString("MainWindow answer for version request %1").arg(result);
        this->ui->lEditTextVersion->setText(msg);
    }
}
