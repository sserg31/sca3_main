import sys
from PyQt4 import QtGui
import PyQt4.QtCore
from PyQt4.QtCore import *
from mainwindow import MyMainWindow


#set process name 
import dl
libc = dl.open('/lib/libc.so.6')
libc.call('prctl', 15, 'shreg', 0, 0, 0) #change process name 'python' on 'shreg'
0
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    app.setStyle("Plastique")
    #set programm locale
    qtTranslator = QTranslator();
    qtTranslator.load("qt_" + QLocale.system().name(),
            QLibraryInfo.location(QLibraryInfo.TranslationsPath));
    app.installTranslator(qtTranslator);

    window = MyMainWindow()
    window.showMaximized()
    sys.exit(app.exec_())
