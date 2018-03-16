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
#include "../../Common/Network/messagecommand.h"
#include "../../Common/Network/messageprotocol.h"
#include "Connection/cconmanager.h"
#include "Connection/cconusersettings.h"
#include "cstaglobalsettings.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetMessagePattern();

    this->ui->lEditIpAddr->setText(g_ConUserSettings.getIPAddr());

    this->ui->lEditSendUserName->setText(g_ConUserSettings.getUserName());

    connect(&g_ConManager, &cConManager::signalNotifyConnectionFinished,
            this, &MainWindow::connectionFinished);
    connect(&g_ConManager, &cConManager::signalNotifyCommandFinished,
            this, &MainWindow::slotNotifyCommandFinished);

    this->ui->lEditReadableName->setText(g_ConUserSettings.getReadableName());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::connectionFinished(qint32 result, const QString msg)
{
    if (result >= ERROR_CODE_NO_ERROR) {
        this->ui->conResult->setStyleSheet("background-color:green");
    } else {
        this->ui->conResult->setStyleSheet("background-color:red");
        qWarning() << QString("Could not connect: %1:%2").arg(result).arg(msg);
    }
}

void MainWindow::slotNotifyCommandFinished(quint32 command, qint32 result)
{
    switch (command) {
    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        if (result == ERROR_CODE_SUCCESS)
            this->ui->lEditTextVersion->setText(g_StaGlobalSettings.getRemoteVersion());
        break;
    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        if (result == ERROR_CODE_SUCCESS) {
            this->ui->lEditTextProperties->setText(QString("0x%1").arg(g_ConUserSettings.getUserProperties(), 8, 16));
            this->ui->lEditReadableName->setText(g_ConUserSettings.getReadableName());
        }
        break;
    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_READNAME:
        if (result == ERROR_CODE_SUCCESS)
            qInfo() << "Update war erfolgreich";
        break;
    case OP_CODE_CMD_REQ::REQ_USER_CHANGE_LOGIN:
        if (result == ERROR_CODE_SUCCESS)
            qInfo() << "Update password war erfolgreich";
        break;
    default:
        break;
    }
}

void MainWindow::on_btnLogin_clicked()
{
    g_ConUserSettings.setIPAddr(this->ui->lEditIpAddr->text());
    QString passWord;
    if (this->ui->lEditTextPassword->text().isEmpty())
        passWord = g_ConUserSettings.getPassWord();
    else {
        passWord = this->ui->lEditTextPassword->text();
        g_ConUserSettings.setSalt("");
    }

    qint32 rCode = g_ConManager.startMainConnection(this->ui->lEditSendUserName->text(), passWord);
    if (rCode != ERROR_CODE_SUCCESS)
        qWarning() << getErrorCodeString(rCode);
}

void MainWindow::on_btnSetReadableName_clicked()
{
    QString name = this->ui->lEditReadableName->text();
    if (name.isEmpty())
        qInfo() << "No readable name";
    else if (name == g_ConUserSettings.getReadableName())
        qInfo() << "Name nicht verändert";
    else
        g_ConUserSettings.startUpdateReadableName(name);
}

void MainWindow::on_btnUdpatePassword_clicked()
{
    QString password = this->ui->lEditTextUpdatePassword->text();
    if (password.isEmpty())
        qInfo() << "No password";
    else
        g_ConUserSettings.startUpdatePassword(password);
}
