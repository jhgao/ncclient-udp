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
    connection.cpp \
    gui/rcvprogressview.cpp \
    gui/rcvprogressscene.cpp \
    protocol/packet.cpp \
    dhudp/dhudp.cpp \
    dhudp/dhudpdecoder.cpp \
    dhudp/decparams.cpp \
    dhudp/dhudprcvqueue.cpp \
    execthread.cpp \
    dhudp/rcvblock.cpp \
    dhudp/fragment.cpp

HEADERS  += clientudpwindow.h \
    datahandler.h \
    connection.h \
    gui/rcvprogressview.h \
    gui/rcvprogressscene.h \
    protocol/protocoltypes.h \
    protocol/ports_define.h \
    protocol/cmd_define.h \
    dhudp/dhudp.h \
    dhudp/dhudpdecoder.h \
    dhudp/dhudpprotocol.h \
    dhudp/decparams.h \
    dhudp/dhudprcvqueue.h \
    execthread.h \
    protocol/protocoltypes.h \
    protocol/ports_define.h \
    protocol/packet.h \
    protocol/cmd_define.h \
    dhudp/consts.h \
    dhudp/rcvblock.h \
    dhudp/fragment.h

FORMS    += clientudpwindow.ui
