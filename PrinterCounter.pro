#-------------------------------------------------
#
# Project created by QtCreator 2017-03-05T16:02:23
#
#-------------------------------------------------

TARGET = PrinterCounter
CONFIG += c++14

SOURCES += main.cpp\
    printer.cpp \
    core.cpp

LIBS += -L "C:/Program Files/Microsoft SDKs/Windows/v7.1A/Lib" -lkernel32 -lWinspool

HEADERS  += \
    printer.h \
    includes.h \
    core.h

CONFIG += console
