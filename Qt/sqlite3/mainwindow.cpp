#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QtSql/qsql.h>
#include "QtSql/QSqlDatabase"
#include "sqlconnect.h"
#include "QtSql/QSqlTableModel"
#include "QDateTime"
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QDateTime>

#define PORT 3000


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,PORT))
    {
        qDebug() << tcpServer->errorString();
        close();
    }
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(connectTcp()));

    ui->dateTimeEdit->setDateTime(QDateTime((QDate(2017,2,26)),QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime((QDate(2017,2,27)),QTime(0,0,0)));
    ui->dateTimeEdit->setDisplayFormat(QString("yyyy-MM-dd hh:mm:ss"));
    ui->dateTimeEdit_2->setDisplayFormat(QString("yyyy-MM-dd hh:mm:ss"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sqlUpdate()
{
    QSqlTableModel *model = new QSqlTableModel();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("DataCom");
    model->select();
    //隐藏行头
    ui->tableView->verticalHeader()->hide();
    //设置表格的单元为只读属性，即不能编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();


    model->database().transaction();
    ui->tableView->clearFocus();
    if(model->submitAll())
    {
        model->database().commit();
    }
    else
    {
        model->database().rollback();
        QMessageBox::warning(this,tr("Update Error"),tr("Update Error!"),QMessageBox::Cancel|QMessageBox::Escape);
    }

}

void MainWindow::insertData()
{
    QSqlQuery query;
    query.prepare("insert into DataCom values(?,?,?,?)");
    query.addBindValue(_dataTime);
    query.addBindValue(_readTemp);
    query.addBindValue(_readHumi);
    query.addBindValue(_readLight);
    query.exec();
}

void MainWindow::sqlUnusual(QString  temp, QString humi, QString light)
{
    if(temp>"25" || temp<"10"
            || humi>"65"|| humi<"30"
            || light > "80")
    {

        QString sqlStr = QString("temp = '%1' ").arg(temp);
        QSqlTableModel *model = new QSqlTableModel;
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->setTable("DataCom");

        model->setFilter(sqlStr);

        model->select();
        //隐藏行头
        ui->tableView_3->verticalHeader()->hide();
        //设置表格的单元为只读属性，即不能编辑
        ui->tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);


        ui->tableView_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_3->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_3->setModel(model);
        ui->tableView_3->resizeColumnsToContents();
        ui->tableView_3->resizeRowsToContents();

        model->database().transaction();
        ui->tableView_3->clearFocus();
        if(model->submitAll())
        {
            model->database().commit();
        }
        else
        {
            model->database().rollback();
            QMessageBox::warning(this,tr("Update Error"),tr("Update Error!"),QMessageBox::Cancel|QMessageBox::Escape);
        }
    }
}

void MainWindow::readTcpSocket()
{
    QByteArray qbaTcp = tcpSocket->readAll();
    QString qba = qbaTcp.trimmed();

    _readTemp = qba.mid(0,3);
    _readHumi = qba.mid(3,3);
    _readLight = qba.mid(6,3);
    _dataTime = qba.mid(9,20);

    insertData();
    sqlUpdate();
    sqlUnusual(_readTemp,_readHumi,_readLight);
    tcpSocket->write(qbaTcp);
}


void MainWindow::connectTcp()
{
    //得到每个连进来的socket
    tcpSocket = tcpServer->nextPendingConnection();

    //有可读的信息
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readTcpSocket()));
}




void MainWindow::on_pushButton_clicked()
{
    sqlUpdate();
    QString dateTimeStart;
    QString dateTimeEnd;

    dateTimeStart = ui->dateTimeEdit->dateTime().toString(QString("yyyy-MM-dd hh:mm:ss"));
    dateTimeEnd = ui->dateTimeEdit_2->dateTime().toString(QString("yyyy-MM-dd hh:mm:ss"));

    QString sqlStr = QString("time between '%1' and '%2'").arg(dateTimeStart).arg(dateTimeEnd);


    QSqlTableModel *model = new QSqlTableModel;
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("DataCom");
    model->setFilter(sqlStr);
    model->select();
    //隐藏行头
    ui->tableView_2->verticalHeader()->hide();
    //设置表格的单元为只读属性，即不能编辑
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    model->database().transaction();
    ui->tableView_2->clearFocus();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_2->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView_2->setModel(model);

    if(model->submitAll())
    {
        model->database().commit();
    }
    else
    {
        model->database().rollback();
        QMessageBox::warning(this,tr("Update Error"),tr("Update Error!"),QMessageBox::Cancel|QMessageBox::Escape);
    }
}
