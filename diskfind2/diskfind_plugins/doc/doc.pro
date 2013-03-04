 TEMPLATE        = lib
 QT             -= gui
 CONFIG         += plugin
 INCLUDEPATH    += ../
 HEADERS         = doc.h
 SOURCES         = doc.cpp
 TARGET          = docplugin
include(../diskfind_plugins.pri)
