#-------------------------------------------------
#
# Project created by QtCreator 2013-03-21T21:01:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yuvdisplay
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    yuv2rgb/yuv420rgb888c.c \
    yuv2rgb/yuv2rgb16tab.c

HEADERS  += dialog.h

FORMS    += dialog.ui
