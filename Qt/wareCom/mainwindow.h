#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTcpSocket>
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
    void readSerialCom();
    void on_opencom_clicked();

    void on_closecom_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *SerialCom;
    QTcpSocket *tcpSocket;
};

#endif // MAINWINDOW_H
