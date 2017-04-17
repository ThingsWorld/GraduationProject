#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T11:15:38
#
#-------------------------------------------------

QT       += core gui
QT    += sql
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sqlite3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    sqlconnect.h

FORMS    += mainwindow.ui
