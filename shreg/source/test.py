#!/usr/bin/env python
"""
main.py - application starter

copyright: (C) 2001, Boudewijn Rempt
email:     boud@rempt.xs4all.nl
"""
import sys, locale
from PyQt4 import QtGui
from PyQt4 import QtCore
import PyQt4.QtCore
from PyQt4.QtCore import *

def main(args):
    app = QtGui.QApplication(sys.argv)

    qtTranslator = QTranslator();
    qtTranslator.load("qt_" + QLocale.system().name(),
            QLibraryInfo.location(QLibraryInfo.TranslationsPath));
    app.installTranslator(qtTranslator);

    sys.exit(app.exec_())
    
if __name__=="__main__":
    main(sys.argv)

