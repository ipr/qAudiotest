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
    FileType.cpp \
    MemoryMappedFile.cpp \
    IffContainer.cpp \
    Iff8svx.cpp \
    RiffWave.cpp \
    RiffContainer.cpp \
    IffAiff.cpp

HEADERS  += mainwindow.h \
    FileType.h \
    MemoryMappedFile.h \
    IffContainer.h \
    Iff8svx.h \
    RiffWave.h \
    RiffContainer.h \
    IffAiff.h \
    AudioFile.h \
    IffChunk.h

FORMS    += mainwindow.ui
