/*
*	This file is part of StamOrga
*   Copyright (C) 2017 Markus Schneider
*
*	This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*	Foobar is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.

*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QMessageBox>
#include <QtCore/QString>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messageprotocol.h"
#include "Connection/cconmanager.h"
#include "Connection/cconsettings.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //    SetMessagePattern();

    this->ui->lEditIpAddr->setText(g_ConSettings.getIPAddr());

    this->ui->lEditSendUserName->setText(g_ConSettings.getUserName());
    //    this->ui->lEditTextPassword->setText(g_ConSettings.getPassWord());


    //    this->m_pConHandling = new cConManager();
    connect(&g_ConManager, &cConManager::sNotifyConnectionFinished,
            this, &MainWindow::connectionFinished);
    //    connect(this->m_pConHandling, &cConManager::sNotifyVersionRequest,
    //            this, &MainWindow::versionRequestFinished);
    //    connect(this->m_pConHandling, &cConManager::sNotifyUserPropertiesRequest,
    //            this, &MainWindow::propertyRequestFinished);
    //    connect(this->m_pConHandling, &cConManager::sNotifyUpdatePasswordRequest,
    //            this, &MainWindow::updatePasswordFinished);
    //    connect(this->m_pConHandling, &cConManager::sNotifyGamesListRequest,
    //            this, &MainWindow::getGamesListFinished);

    //    this->m_pGlobalData = new GlobalData();
    //    this->m_pGlobalData->loadGlobalSettings();

    //
    //
    //    this->ui->spBoxPort->setValue(this->m_pGlobalData->conMasterPort());

    //    this->ui->btnUdpatePassword->setEnabled(false);
    //    this->ui->lEditTextUpdatePassword->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::on_btnSendData_clicked()
//{
//    this->m_pConHandling->setGlobalData(this->m_pGlobalData);
//    QString username = this->ui->lEditSendUserName->text();
//    QString password = this->ui->lEditTextPassword->text();
//    this->m_pGlobalData->setIpAddr(this->ui->lEditIpAddr->text());
//    this->m_pGlobalData->setConMasterPort(this->ui->spBoxPort->value());
//    if (this->m_pConHandling->startMainConnection(username, password))
//    {
//        this->ui->btnSendData->setEnabled(false);
//    }
//}

void MainWindow::connectionFinished(qint32 result, const QString msg)
{
    if (result >= ERROR_CODE_NO_ERROR) {
        this->ui->conResult->setStyleSheet("background-color:green");
        //        if (!this->m_pConHandling->startGettingInfo())
        //            this->ui->btnSendData->setEnabled(true);
        //        this->ui->btnUdpatePassword->setEnabled(true);
        //        this->ui->lEditTextUpdatePassword->setEnabled(true);
    } else {
        this->ui->conResult->setStyleSheet("background-color:red");
        qWarning() << QString("Could not connect: %1:%2").arg(result).arg(msg);
        //        this->ui->btnSendData->setEnabled(true);
    }
}

//void MainWindow::versionRequestFinished(qint32 result, QString msg)
//{
//    this->ui->btnSendData->setEnabled(true);

//    if (result == ERROR_CODE_NEW_VERSION) {
//        QMessageBox msgBox;
//        msgBox.setText(msg);
//        msgBox.exec();
//    } else
//        this->ui->lEditTextVersion->setText(msg);
//}

//void MainWindow::propertyRequestFinished(qint32 result, quint32 value)
//{
//    if (result > ERROR_CODE_NO_ERROR) {
//        this->ui->lEditTextProperties->setText("0x" + QString::number(value, 16));
//    } else {
//        this->ui->lEditTextProperties->setText(getErrorCodeString(result));
//    }
//}

//void MainWindow::on_btnUdpatePassword_clicked()
//{
//    this->m_pConHandling->startUpdatePassword(this->ui->lEditTextUpdatePassword->text());
//}

//void MainWindow::updatePasswordFinished(qint32 result)
//{
//    if (result >= ERROR_CODE_NO_ERROR)
//        this->ui->lEditTextUpdatePassword->setText("SUCCESS");
//    else
//        this->ui->lEditTextUpdatePassword->setText(getErrorCodeString(result));
//}


//void MainWindow::on_btnGetGamesList_clicked()
//{
//    this->m_pConHandling->startGetGamesList();
//}

//void MainWindow::getGamesListFinished(qint32 result)
//{
//    qDebug() << QString("Getting Games list request answer: %1 ").arg(result);
//}

void MainWindow::on_btnLogin_clicked()
{
    g_ConSettings.setIPAddr(this->ui->lEditIpAddr->text());
    QString passWord;
    if (this->ui->lEditTextPassword->text().isEmpty())
        passWord = g_ConSettings.getPassWord();
    else {
        passWord = this->ui->lEditTextPassword->text();
        g_ConSettings.setSalt("");
    }

    qint32 rCode = g_ConManager.startMainConnection(this->ui->lEditSendUserName->text(), passWord);
    if (rCode != ERROR_CODE_SUCCESS)
        qWarning() << getErrorCodeString(rCode);
}
