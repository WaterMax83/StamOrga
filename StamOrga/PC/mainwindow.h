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

private:
    Ui::MainWindow *ui;

    ConnectionHandling *m_pConHandling;
    GlobalData          *m_pGlobalData;
};

#endif // MAINWINDOW_H
