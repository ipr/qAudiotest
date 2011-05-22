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
    FileType.cpp \
    MemoryMappedFile.cpp \
    IffContainer.cpp \
    Iff8svx.cpp \
    RiffWave.cpp \
    RiffContainer.cpp \
    IffAiff.cpp

HEADERS  += mainwindow.h \
    wavfile.h \
    utils.h \
    FileType.h \
    MemoryMappedFile.h \
    IffContainer.h \
    Iff8svx.h \
    RiffWave.h \
    RiffContainer.h \
    IffAiff.h

FORMS    += mainwindow.ui
