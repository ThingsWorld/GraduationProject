#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlQuery>
#include <QTableView>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void sqlUpdate();
    void insertData();
    void sqlUnusual(QString ,QString , QString);
protected slots:
    void connectTcp();
    void readTcpSocket();

private:
    Ui::MainWindow *ui;
protected:
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    QString _dataTime;
    QString _readTemp;
    QString _readHumi;
    QString _readLight;
private slots:
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
