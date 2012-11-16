# -*- coding: utf-8 -*-
from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

from conversion import *

class MyTreeView(QTreeView):
    def __init__(self, *args):
        QTreeView.__init__(self, *args)
        #self.setDragEnabled(True)
        #elf.setAcceptDrops(True)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)

        self.ContextAdd = QtGui.QAction(decode(_('Add')), self)
        self.connect(self.ContextAdd, QtCore.SIGNAL("triggered()"), self.contextAdd)
        
        #self.setColumnWidth(1,100)

    def event(self, event):
        if (event.type()==QEvent.KeyPress) and (event.key()==Qt.Key_Insert):
            self.emit(SIGNAL("insertPressed"))
            return True

        return QTreeView.event(self, event)

    def contextAdd(self):
         self.emit(SIGNAL("addTo"))

    def Menu(self, point):
        menu = QMenu(self)
        menu.addAction(self.ContextAdd)
        menu.popup(QCursor.pos()) 
