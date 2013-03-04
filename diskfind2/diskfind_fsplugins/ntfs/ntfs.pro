TEMPLATE        = lib
QT             -= gui
CONFIG         += plugin
INCLUDEPATH    += ../ \
                  /usr/include/ntfs

DEFINES        +=  HAVE_CONFIG_H

HEADERS         = utils.h \
                  ntfsundelete.h \
                  ntfs.h \
                  config.h

SOURCES         = utils.cpp \
                  ntfs.cpp

LIBS           += -lntfs

TARGET          = ntfsplugin
include(../diskfind_fsplugins.pri)
