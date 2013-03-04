 TARGET          = pdfplugin

 include(../diskfind_plugins.pri)
 
 TEMPLATE        = lib
 QT             -= gui
 CONFIG         += plugin
 INCLUDEPATH    += ../
 HEADERS         = pdf.h
 SOURCES         = pdf.cpp
