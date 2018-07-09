#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T12:39:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatClient
TEMPLATE = app
CONFIG += c++11
INCLUDEPATH += ../ChatServer/


SOURCES += main.cpp\
        mainwindow.cpp \
    tcpsocket.cpp \
    chatclient.cpp

HEADERS  += mainwindow.h \
    tcpsocket.h \
    chatclient.h \
    uiinterface.h

FORMS    += mainwindow.ui

