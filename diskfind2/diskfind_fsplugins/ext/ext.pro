TEMPLATE        = lib
QT             -= gui
CONFIG         += plugin
INCLUDEPATH    += ../ \
                  /usr/include/ext2fs

HEADERS         = ext.h

SOURCES         = ext.cpp

LIBS           += -lext2fs

TARGET          = extplugin
include(../diskfind_fsplugins.pri)
