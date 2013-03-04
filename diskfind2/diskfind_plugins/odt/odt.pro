 TARGET          = odtplugin

 include(../diskfind_plugins.pri)
 
 TEMPLATE        = lib
 QT             -= gui
 CONFIG         += plugin
 INCLUDEPATH    += ../
 HEADERS         = odt.h
 SOURCES         = odt.cpp
