#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../connectionhandling.h"
#include "../globaldata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnSendData_clicked();
    void connectionFinished(qint32 result);
    void versionRequestFinished(qint32 result, QString msg);
    void propertyRequestFinished(qint32 result, quint32 value);
    void updatePasswordFinished(qint32 result);
    void getGamesListFinished(qint32 result);

    void on_btnUdpatePassword_clicked();

    void on_btnGetGamesList_clicked();

private:
    Ui::MainWindow *ui;

    ConnectionHandling *m_pConHandling;
    GlobalData          *m_pGlobalData;
};

#endif // MAINWINDOW_H
