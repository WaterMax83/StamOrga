#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../connectionhandling.h"

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
    void ConnectionFinished();

private:
    Ui::MainWindow *ui;

    ConnectionHandling *m_pMainCon;
};

#endif // MAINWINDOW_H
