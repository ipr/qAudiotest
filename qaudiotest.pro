#-------------------------------------------------
#
# Project created by QtCreator 2011-02-15T23:50:39
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = qaudiotest
TEMPLATE = app

DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    wavfile.cpp \
    utils.cpp \

HEADERS  += mainwindow.h \
    wavfile.h \
    utils.h \

FORMS    += mainwindow.ui
