# -*- coding: utf-8 -*-
from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

from conversion import *

class MyListWidget(QListWidget):
    def __init__(self, *args):
        QListWidget.__init__(self, *args)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)

        #self.setDragEnabled(True)
        #self.setAcceptDrops(True)
        self.ContextRemove = QtGui.QAction(decode(_('Remove')), self)
        self.connect(self.ContextRemove, QtCore.SIGNAL("triggered()"), self.contextRemove)
    
    def event(self, event):
        if (event.type()==QEvent.KeyPress) and (event.key()==Qt.Key_Delete):
            self.emit(SIGNAL("deletePressed"))
            return True

        return QListWidget.event(self, event)
    
    def contextRemove(self):
        self.emit(SIGNAL("removeFrom"))

    def Menu(self, point):
        menu = QMenu(self)
        menu.addAction(self.ContextRemove)
        menu.popup(QCursor.pos()) 
