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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QStandardItemModel>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void connectionFinished(const qint32 result);
    void slotNotifyCommandFinished(const quint32 command, const qint32 result, const qint32 subCmd);

    void on_btnLogin_clicked();

    void on_btnSetReadableName_clicked();

    void on_btnUdpatePassword_clicked();

    void on_btnRefreshControl_clicked();

    void on_btnSaveControl_clicked();

    void on_btnConsole_clicked();

    void on_lEditConsoleCommand_returnPressed();

    void on_btnSendNotify_clicked();

    void on_btnRefreshWebPage_clicked();

    void on_btnAddWebPage_clicked();

    void on_lViewWebPageList_clicked(const QModelIndex& index);

    void on_btnSendWebPageData_clicked();

private:
    Ui::MainWindow* ui;

    QStandardItemModel* m_listModelWebpage = NULL;
    QString             m_lastControlCommand;
    QString             m_lastWebPageCommand;
    void                sendConsoleCommand();
};

#endif // MAINWINDOW_H
