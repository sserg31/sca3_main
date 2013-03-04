 TEMPLATE        = lib
 QT             -= gui
 CONFIG         += plugin
 INCLUDEPATH    += ../
 HEADERS         = \
    docx.h
 SOURCES         = \
    docx.cpp
 TARGET          = docxplugin
include(../diskfind_plugins.pri)

xlsx2csv.path += /usr/bin/
xlsx2csv.files += xlsx2csv.py

INSTALLS += xlsx2csv
