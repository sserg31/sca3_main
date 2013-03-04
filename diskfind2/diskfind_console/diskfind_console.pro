# -------------------------------------------------
# Project created by QtCreator 2010-02-09T13:38:59
# -------------------------------------------------
TARGET = ../diskfind2_build/diskfind2/usr/bin/diskfind_console
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
QT -= gui xml
SOURCES += main.cpp \
    diskfind.cpp \
    typed_parser.cpp \
    signature.cpp \
    support.cpp
HEADERS += diskfind.h \
    typed_parser.h \
    support.h \
    typed_parser.h \
    signature.h \
    ../diskfind_plugins/basetypedparser.h

target.path += /usr/bin
target.files += $$(TARGET)

INSTALLS += target
