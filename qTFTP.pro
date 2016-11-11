#-------------------------------------------------
#
# Project created by QtCreator 2014-03-20T23:26:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qTFTP
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    dialoggetfilename.cpp

HEADERS  += dialog.h \
    dialoggetfilename.h \
    tftp.h

FORMS    += dialog.ui \
    dialoggetfilename.ui
