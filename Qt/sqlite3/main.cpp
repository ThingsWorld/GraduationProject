#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <sqlconnect.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(!sqlconnect())
    {
        qDebug()<<"faild sql connect"<<endl;
        return -1;
    }

    w.show();
    return a.exec();
}
