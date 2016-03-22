#-------------------------------------------------
#
# Project created by QtCreator 2015-11-25T19:19:13
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ManualBase
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    manualdialog.cpp \
    modeleditdialog.cpp \
    database.cpp \
    htmlmaker.cpp

HEADERS  += mainwindow.h \
    manualdialog.h \
    modeleditdialog.h \
    database.h \
    htmlmaker.h

FORMS    += mainwindow.ui \
    manualdialog.ui \
    modeleditdialog.ui

RESOURCES += \
    resurs.qrc
