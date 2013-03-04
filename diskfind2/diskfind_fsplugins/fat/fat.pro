 TEMPLATE        = lib
 QT             -= gui
 CONFIG         += plugin
 INCLUDEPATH    += ../
 HEADERS         = fat.h \
    lfn.h \
    io.h \
    file.h \
    fat1.h \
    dosfsck.h \
    common.h \
    check.h \
    boot.h
 SOURCES         = fat.cpp \
    lfn.cpp \
    io.cpp \
    file.cpp \
    fat1.cpp \
    common.cpp \
    check.cpp \
    boot.cpp
 TARGET          = fatplugin
include(../diskfind_fsplugins.pri)
