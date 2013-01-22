#-------------------------------------------------
#
# Project created by QtCreator 2013-01-22T22:14:17
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClientUDP
TEMPLATE = app


SOURCES += main.cpp\
        clientudpwindow.cpp \
    datahandler.cpp \
    connectionthread.cpp \
    connection.cpp \
    gui/rcvprogressview.cpp \
    gui/rcvprogressscene.cpp \
    protocol/rawblock.cpp \
    protocol/packet.cpp \
    dhudp/dhudp.cpp

HEADERS  += clientudpwindow.h \
    datahandler.h \
    connectionthread.h \
    connection.h \
    gui/rcvprogressview.h \
    gui/rcvprogressscene.h \
    protocol/rawblock.h \
    protocol/protocoltypes.h \
    protocol/ports_define.h \
    protocol/packet.h \
    protocol/cmd_define.h \
    dhudp/dhudp.h

FORMS    += clientudpwindow.ui
