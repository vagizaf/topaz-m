#-------------------------------------------------
#
# Project created by QtCreator 2012-03-04T23:24:55
#
#-------------------------------------------------

QT += core gui script
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
CONFIG += c++11
TARGET = untitled2

TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
         ../../qcustomplot.cpp \
    updategraph.cpp \
    dialog.cpp \
    options.cpp

HEADERS  += mainwindow.h \
         ../../qcustomplot.h \
    updategraph.h \
    dialog.h \
    options.h

FORMS    += mainwindow.ui \
    dialog.ui \
    options.ui

DISTFILES +=

