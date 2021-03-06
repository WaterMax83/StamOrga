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
#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../Common/General/globalfunctions.h"
#include "../../Common/Network/messagecommand.h"
#include "../../Common/Network/messageprotocol.h"
#include "../Data/cdatausermanager.h"
#include "Connection/cconmanager.h"
#include "Connection/cconusersettings.h"
#include "Data/cdataconsolemanager.h"
#include "Data/cdatawebpagemanager.h"
#include "PC/cpccontrolmanager.h"
#include "cstaglobalsettings.h"
#include "cstaversionmanager.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SetMessagePattern();

    this->ui->lEditIpAddr->setText(g_ConUserSettings->getIPAddr());

    this->ui->lEditSendUserName->setText(g_ConUserSettings->getUserName());

    connect(g_ConManager, &cConManager::signalNotifyConnectionFinished,
            this, &MainWindow::connectionFinished);
    connect(g_ConManager, &cConManager::signalNotifyCommandFinished,
            this, &MainWindow::slotNotifyCommandFinished);

    this->ui->lEditReadableName->setText(g_ConUserSettings->getReadableName());

    //    this->ui->txtEditStatistic->setText(g_PCControlManager->getStastistic());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::connectionFinished(const qint32 result)
{
    if (result >= ERROR_CODE_NO_ERROR) {
        this->ui->conResult->setStyleSheet("background-color:green");
    } else {
        this->ui->conResult->setStyleSheet("background-color:red");
        qWarning() << QString("Could not connect: %1").arg(result);
    }
}

void MainWindow::slotNotifyCommandFinished(const quint32 command, const qint32 result, const qint32 subCmd)
{
    Q_UNUSED(subCmd);

    switch (command) {
    case OP_CODE_CMD_REQ::REQ_GET_VERSION:
        if (result == ERROR_CODE_SUCCESS)
            this->ui->lEditTextVersion->setText(g_StaVersionManager->getRemoteVersion());
        break;
    case OP_CODE_CMD_REQ::REQ_GET_USER_PROPS:
        if (result == ERROR_CODE_SUCCESS) {
            this->ui->lEditTextProperties->setText(QString("0x%1").arg(g_ConUserSettings->getUserProperties(), 8, 16));
            this->ui->lEditReadableName->setText(g_ConUserSettings->getReadableName());
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
    case OP_CODE_CMD_REQ::REQ_SEND_CONSOLE_CMD:
        if (result == ERROR_CODE_SUCCESS)
            this->ui->txtConsole->setPlainText(g_DataConsoleManager->getLastConsoleOutput());
    case OP_CODE_CMD_REQ::REQ_CMD_CONTROL:
        if (result == ERROR_CODE_SUCCESS) {
            if (this->m_lastControlCommand == "refresh" || this->m_lastControlCommand == "save") {
                this->ui->txtEditStatistic->setText(g_PCControlManager->getStastistic());
                this->ui->txtEditOnlineGames->setText(g_PCControlManager->getOnlineGames());
                QString login, password, addr;
                g_PCControlManager->getStmpData(login, password, addr);
                this->ui->lEditSmtpLogin->setText(login);
                this->ui->lEditSmtpPassword->setText(password);
                this->ui->txtEditSmtpEmailAdresses->setText(addr);
                this->ui->btnSaveControl->setEnabled(true);
            } else if (this->m_lastControlCommand == "notify") {
                QString header  = this->ui->lEditNotifyHeader->text();
                QString body    = this->ui->txtEditNotifyBody->toPlainText();
                QString time    = QDateTime::currentDateTime().toString("dd.MM hh:mm:ss");
                QString message = QString("%1: %2 - %3\n").arg(time, header, body);
                this->ui->txtEditNotifyLastMessage->append(message);
                this->ui->lEditNotifyHeader->setText("");
                this->ui->txtEditNotifyBody->setText("");
            }
        } else {
            this->ui->btnSaveControl->setEnabled(false);
        }
        break;
    case OP_CODE_CMD_REQ::REQ_CMD_STADIUM:
        if (result == ERROR_CODE_SUCCESS) {
            if (this->m_lastWebPageCommand == "list") {
                if (this->m_listModelWebpage == NULL)
                    this->m_listModelWebpage = new QStandardItemModel();
                for (int i = 0; i < this->m_listModelWebpage->rowCount(); i++)
                    delete this->m_listModelWebpage->item(i);
                this->m_listModelWebpage->clear();

                for (int i = 0; i < g_DataWebPageManager->getWebListLength(); i++) {
                    TextDataItem*  pDataItem = g_DataWebPageManager->getWebDataFromArrayIndex(i);
                    QStandardItem* pItem     = new QStandardItem();
                    pItem->setData(pDataItem->index());
                    if (pDataItem->header().isEmpty())
                        pItem->setText(pDataItem->timestampReadableLine());
                    else
                        pItem->setText(pDataItem->header());
                    this->m_listModelWebpage->setItem(i, 0, pItem);
                }

                this->ui->lViewWebPageList->setModel(this->m_listModelWebpage);
            } else if (this->m_lastWebPageCommand == "add") {
                this->m_lastWebPageCommand = "list";
                g_DataWebPageManager->startListWebPageData();
            } else if (this->m_lastWebPageCommand == "load") {
                TextDataItem* pItem = g_DataWebPageManager->getCurrentEditedItem();
                if (pItem == NULL)
                    break;
                this->ui->lEditWebPageHeader->setText(pItem->header());
                this->ui->txtEditWebPageLink->setPlainText(pItem->info());
                qInfo() << "Loaded item";
            } else if (this->m_lastWebPageCommand == "set") {
                qInfo() << "Set Item";
                this->m_lastWebPageCommand = "list";
                g_DataWebPageManager->startListWebPageData();
            }
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_btnLogin_clicked()
{
    g_ConUserSettings->setIPAddr(this->ui->lEditIpAddr->text());
    QString passWord;
    if (this->ui->lEditTextPassword->text().isEmpty())
        passWord = g_ConUserSettings->getPassWord();
    else {
        passWord = this->ui->lEditTextPassword->text();
        g_ConUserSettings->setSalt("");
    }

    qint32 rCode = g_ConManager->startMainConnection(this->ui->lEditSendUserName->text(), passWord);
    if (rCode != ERROR_CODE_SUCCESS)
        qWarning() << getErrorCodeString(rCode);
}

void MainWindow::on_btnSetReadableName_clicked()
{
    QString name = this->ui->lEditReadableName->text();
    if (name.isEmpty())
        qInfo() << "No readable name";
    else if (name == g_ConUserSettings->getReadableName())
        qInfo() << "Name nicht verändert";
    else
        g_DataUserManager->startUpdateReadableName(name);
}

void MainWindow::on_btnUdpatePassword_clicked()
{
    QString password = this->ui->lEditTextUpdatePassword->text();
    if (password.isEmpty())
        qInfo() << "No password";
    else
        g_DataUserManager->startUpdatePassword(password);
}

void MainWindow::on_btnRefreshControl_clicked()
{
    g_PCControlManager->refreshControlList();
    this->ui->btnSaveControl->setEnabled(false);
    this->ui->txtEditStatistic->clear();
    this->ui->txtEditOnlineGames->clear();
    this->ui->txtEditSmtpEmailAdresses->clear();
    this->ui->lEditSmtpLogin->clear();
    this->ui->lEditSmtpPassword->clear();
    this->m_lastControlCommand = "refresh";
}

void MainWindow::on_btnSaveControl_clicked()
{
    QString login    = this->ui->lEditSmtpLogin->text();
    QString password = this->ui->lEditSmtpPassword->text();
    g_PCControlManager->setStatistic(this->ui->txtEditStatistic->toPlainText());
    g_PCControlManager->setOnlineGames(this->ui->txtEditOnlineGames->toPlainText());
    g_PCControlManager->setSmtpData(login, password, this->ui->txtEditSmtpEmailAdresses->toPlainText());
    g_PCControlManager->saveControlList();
    this->ui->btnSaveControl->setEnabled(false);
    this->ui->txtEditStatistic->clear();
    this->ui->txtEditOnlineGames->clear();
    this->ui->txtEditSmtpEmailAdresses->clear();
    this->ui->lEditSmtpLogin->clear();
    this->ui->lEditSmtpPassword->clear();
    this->m_lastControlCommand = "save";
}

void MainWindow::on_btnSendNotify_clicked()
{
    QString header = this->ui->lEditNotifyHeader->text();
    QString body   = this->ui->txtEditNotifyBody->toPlainText();
    g_PCControlManager->sendGeneralNotification(header, body);
    this->m_lastControlCommand = "notify";
}

void MainWindow::on_btnConsole_clicked()
{
    this->sendConsoleCommand();
}

void MainWindow::on_lEditConsoleCommand_returnPressed()
{
    this->sendConsoleCommand();
}

void MainWindow::sendConsoleCommand()
{
    g_DataConsoleManager->startSendConsoleCommand(this->ui->lEditConsoleCommand->text().trimmed());

    this->ui->lEditConsoleCommand->clear();
    this->ui->txtConsole->clear();
}

void MainWindow::on_btnRefreshWebPage_clicked()
{
    this->m_lastWebPageCommand = "list";
    g_DataWebPageManager->startListWebPageData();
}

void MainWindow::on_btnAddWebPage_clicked()
{
    this->m_lastWebPageCommand = "add";
    g_DataWebPageManager->startAddWebPage();
}

void MainWindow::on_lViewWebPageList_clicked(const QModelIndex& index)
{
    this->m_lastWebPageCommand = "load";
    g_DataWebPageManager->startLoadWebPage(this->m_listModelWebpage->item(index.row())->data().toInt(), 0);
}

void MainWindow::on_btnSendWebPageData_clicked()
{
    this->m_lastWebPageCommand = "set";
    g_DataWebPageManager->startSetWebPage(this->ui->lEditWebPageHeader->text(), this->ui->txtEditWebPageLink->toPlainText());
}
