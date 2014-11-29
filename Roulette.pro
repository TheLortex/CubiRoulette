#-------------------------------------------------
#
# Project created by QtCreator 2014-11-27T17:59:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Roulette
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    roulettesimulator.cpp

HEADERS  += mainwindow.h \
    roulettesimulator.h

FORMS    += mainwindow.ui

LIBS += -lBox2D
