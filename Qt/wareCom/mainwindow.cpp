#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>
#include <QString>
#include <QtNetwork>
#include <QTcpSocket>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->closecom->setEnabled(false);
    ui->lineEdit_5->setText("192.168.1.202");
    ui->lineEdit_6->setText("3000");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSerialCom()
{
    //读取串口得到的数据
    QByteArray readarray = SerialCom->readAll();
    QString qba = readarray.trimmed();
    ui->lineEdit->setText(qba.mid(0,3)+"C");
    ui->lineEdit_2->setText(qba.mid(3,3)+"%");
    ui->lineEdit_3->setText(qba.mid(6,3));
    tcpSocket->write(readarray);


    //获取当地时间
    QString datatime = QDateTime::currentDateTime()\
            .toString(" yyyy-MM-dd hh:mm:ss ");
    QByteArray time = datatime.toLatin1();
    tcpSocket->write(time);

    ui->lineEdit_4->setText(datatime);

}

void MainWindow::on_opencom_clicked()
{
    QString ip = ui->lineEdit_5->text();
    QString port = ui->lineEdit_6->text();
    tcpSocket = new QTcpSocket(this);
    tcpSocket->abort();
    tcpSocket->connectToHost(QHostAddress(ip),port.toInt());

    //设置串口选择
    QString portname = ui->portcomboBox->currentText();
    SerialCom = new QSerialPort(portname);
    SerialCom->open(QIODevice::ReadWrite); //打开串口

    //设置波特率
    QString baudname = ui->bauddatacomboBox->currentText();
    if(baudname=="9600")
    {
        SerialCom->setBaudRate(QSerialPort::Baud9600);

    }
    else if(baudname=="115200")
    {
        SerialCom->setBaudRate(QSerialPort::Baud115200);
    }

    //设置停止位
    QString stopname = ui->stopBitscomboBox->currentText();
    if(stopname=="1")
    {
        SerialCom->setStopBits(QSerialPort::OneStop);
    }
    else if(stopname=="1.5")
    {
        SerialCom->setStopBits(QSerialPort::OneAndHalfStop);
    }
    else if(stopname=="2")
    {
        SerialCom->setStopBits(QSerialPort::TwoStop);
    }

    //设置数据位
    QString dataname = ui->dataBitscomboBox->currentText();
    if(dataname=="5")
    {
        SerialCom->setDataBits(QSerialPort::Data5);
    }
    else if(dataname=="6")
    {
        SerialCom->setDataBits(QSerialPort::Data6);
    }
    else if(dataname=="7")
    {
        SerialCom->setDataBits(QSerialPort::Data7);
    }
    else if(dataname=="8")
    {
        SerialCom->setDataBits(QSerialPort::Data8);
    }


    //设置奇偶校验
    QString parityname = ui->paritycomboBox->currentText();
    if(parityname=="无")
    {
        SerialCom->setParity(QSerialPort::NoParity);
    }
    else if(parityname=="奇校验")
    {
        SerialCom->setParity(QSerialPort::OddParity);
    }
    else if(parityname=="偶校验")
    {
        SerialCom->setParity(QSerialPort::EvenParity);
    }
    //当有接收到串口数据时触发槽函数
    connect(SerialCom,SIGNAL(readyRead()),this,SLOT(readSerialCom()));

    //当打开串口时，只有关闭串口可用，其余组合框不能使用
    ui->closecom->setEnabled(true);
    ui->opencom->setEnabled(false);
    ui->bauddatacomboBox->setEnabled(false);
    ui->dataBitscomboBox->setEnabled(false);
    ui->paritycomboBox->setEnabled(false);
    ui->portcomboBox->setEnabled(false);
    ui->stopBitscomboBox->setEnabled(false);
}

void MainWindow::on_closecom_clicked()
{
    SerialCom->close();
    //当关闭串口时，恢复其余的组合框使用
    ui->closecom->setEnabled(false);
    ui->opencom->setEnabled(true);
    ui->bauddatacomboBox->setEnabled(true);
    ui->dataBitscomboBox->setEnabled(true);
    ui->paritycomboBox->setEnabled(true);
    ui->portcomboBox->setEnabled(true);
    ui->stopBitscomboBox->setEnabled(true);
}










