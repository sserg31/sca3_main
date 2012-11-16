#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import re
import signal
import time
import subprocess
import commands
from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

from treeView import MyTreeView
from listWidget import MyListWidget
from shred import ThreadShred
from conversion import *



systemDirs = [
                "/",
                "/usr",
                "/etc",
                "/var",
                "/tmp",
                "/dev",
                "/bin",
                "/sbin",
                "/proc",
                "/sys",
                "/media",
                "/mount",
                "/boot"
             ]


class MyMainWindow(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QMainWindow.__init__(self)
        self.setWindowTitle(decode(_("shreg - cleaning information")))

        self.model = QtGui.QDirModel(self)
        self.model.setFilter(QtCore.QDir.Dirs | QtCore.QDir.Files | QtCore.QDir.NoDotAndDotDot)
        self.model.setReadOnly(True)

        self.tree = MyTreeView()
        self.tree.setModel(self.model)
        self.tree.setColumnWidth(0,300)
        
        self.list = MyListWidget()
        self.list2 = MyListWidget()

        self.pbarStep = QProgressBar()
        self.pbarStep.setVisible(False)
        self.pbarStep.setValue(1)
        self.pbarStep.setMaximum(1)
        self.pbarStep.setFormat("")

        self.textArea = QTextEdit()
        self.textArea.setReadOnly(True)
        self.textArea.setVisible(False)   

        self.pbarAll = QProgressBar()
        self.pbarAll.setVisible(True)
        self.pbarAll.setValue(0)
        self.pbarAll.setMaximum(100)
        self.pbarAll.setFormat(decode(_("Add files to the list and Push 'Clean'")))

        self.ledit = QLineEdit()
        self.ledit.setVisible(False)
        self.ledit.setReadOnly(True)
        self.grid = QtGui.QGridLayout()
        

#        self.lbl1 = QtGui.QLabel(decode(_("Files Tree")))
#        self.lbl2 = QtGui.QLabel(decode(_("Files to shred")))
#        self.lbl3 = QtGui.QLabel(decode(_("Devices for shred free space")))   

        self.tabMode = QtGui.QTabWidget()
        self.tabMode.addTab(self.tree, decode(_("Files to shred")))
        self.tabMode.addTab(self.list2, decode(_("Devices for shred free space")))
        self.tabMode.setTabIcon(0, QtGui.QIcon("/usr/share/shreg/files.png"))
        self.tabMode.setTabIcon(1, QtGui.QIcon("/usr/share/shreg/diveces.png"))

        self.grid.addWidget(self.tabMode, 1, 1)
        self.grid.addWidget(self.list, 1, 2)
        self.grid.addWidget(self.textArea, 2,1, 2,2)
        self.grid.addWidget(self.pbarStep, 4, 1)
        self.grid.addWidget(self.ledit, 4, 2)
        self.grid.addWidget(self.pbarAll, 5, 1, 5, 2);

        self.widget = QtGui.QWidget(self)
        self.widget.setLayout(self.grid)
        self.setCentralWidget(self.widget)
 
        self.toolBar = self.addToolBar(self.tr("&Location Toolbar"))
        self.toolBar.setMovable(False)
        self.toolBar.setIconSize(QSize(32,32))
        self.toolBar.setFloatable(False)
        self.homeAction = self.toolBar.addAction(decode(_("Home")), self.goHome)
        self.homeAction.setIcon(QIcon("/usr/share/shreg/home.png")) 
        self.refreshAction = self.toolBar.addAction(decode(_("Refresh")), self.refresh)
        self.refreshAction.setIcon(QIcon("/usr/share/shreg/refresh.png"))
        
        self.menuView = QtGui.QMenu()
        self.viewAllAction = self.menuView.addAction(decode(_("Show all")), self.viewAll)
        self.notViewHiddenAction = self.menuView.addAction(decode(_("Show all, without hidden")), self.notViewHidden)
        self.viewOnlyDirsAction = self.menuView.addAction(decode(_("Show only dirs")), self.viewOnlyDirs)
        self.notViewHiddenAction.setCheckable(True)
        self.notViewHiddenAction.setChecked(True)
     
        self.toolBtn = QtGui.QToolButton()
        self.toolBtn.setToolTip(decode(_("View")))
        self.toolBtn.setIcon(QIcon("/usr/share/shreg/view.png"))
        self.toolBar.addWidget(self.toolBtn)
        self.toolBtn.setMenu(self.menuView) 
        self.countSpBox = QtGui.QSpinBox()
        self.countSpBox.setToolTip(decode(_("iterations")))
        self.countSpBox.setMinimum(1)
        self.countSpBox.setMaximum(35)
        self.toolBar.addWidget(self.countSpBox)
        self.toolBar.addSeparator() 

        self.addAction = self.toolBar.addAction(decode(_("Add to list")), self.addTo)
        self.addAction.setIcon(QIcon("/usr/share/shreg/add.png"))
        self.addAction.setShortcut('Ctrl+A')
        self.removeAction = self.toolBar.addAction(decode(_("Delete from list")), self.removeFrom)
        self.removeAction.setIcon(QIcon("/usr/share/shreg/remove.png"))
        self.removeAction.setShortcut('Ctrl+D') 
        self.clearAction = self.toolBar.addAction(decode(_("Clean the list")), self.clearTo)     
        self.clearAction.setIcon(QIcon("/usr/share/shreg/clear.png"))
        self.toolBar.addSeparator() 

        self.SaSAction = self.toolBar.addAction(decode(_("Start/Stop")), self.SaS)
        self.SaSAction.setIcon(QIcon("/usr/share/shreg/start.png"))
        self.exitAction= self.toolBar.addAction(decode(_("Exit")), self.closeEvent) 
        self.exitAction.setIcon(QIcon("/usr/share/shreg/exit.png"))  
        self.exitAction.setShortcut('Ctrl+E')
        self.toolBar.addSeparator()

           
        self.logAction= self.toolBar.addAction(decode(_("Log")), self.openLog) 
        self.logAction.setIcon(QIcon("/usr/share/shreg/log.png"))  

        self.currentPath = QtCore.QDir.homePath()
        self.tree.setCurrentIndex(self.model.index(self.currentPath))

        self.freeSpace = {}
        self.devices = {}

        self.paths = {}
        self.isStartNow = False
        self.shred = None
        

        # tray icon
        self.trayIconMenu = QtGui.QMenu(self)
        self.trayIconMenu.addAction(self.exitAction)
        self.trayIconPixmap = QtGui.QPixmap('/usr/share/pixmaps/shreg.png') # файл иконки
        self.trayIcon = QtGui.QSystemTrayIcon(self)
        self.trayIcon.setContextMenu(self.trayIconMenu)
        self.trayIcon.setIcon(QtGui.QIcon(self.trayIconPixmap))
        self.trayIcon.show()


        # connections
        self.connect(self.tree, QtCore.SIGNAL("insertPressed"), self.addTo)
        self.connect(self.list, QtCore.SIGNAL("deletePressed"), self.removeFrom)
        self.connect(self.tree, QtCore.SIGNAL("customContextMenuRequested(const QPoint &)"), self.tree.Menu) 
        self.connect(self.list, QtCore.SIGNAL("customContextMenuRequested(const QPoint &)"), self.list.Menu) 
        self.connect(self.list, QtCore.SIGNAL("removeFrom"), self.removeFrom) 
        self.connect(self.tree, QtCore.SIGNAL("addTo"), self.addTo) 
        self.connect(self, QtCore.SIGNAL("setCommand"), self.setCommand)
        self.connect(self, QtCore.SIGNAL("setTextOfdd"), self.setTextOfdd)
        self.connect(self.exitAction,QtCore.SIGNAL('triggered()'),QtCore.SLOT('close()'))        
        self.connect(self.toolBtn, QtCore.SIGNAL('clicked()'), self.showMenuView)


        self.refresh()

    def getVolumeInfo(self, filename):
        stats = subprocess.Popen(["df", "-h", filename], stdout=subprocess.PIPE).communicate()[0]
        info = decode(_("(all/used/free)"))
        return stats.splitlines()[1].split()[1:4], info

    def updateDevices(self):
        devices = []
        self.devices = {}
        self.list2.clear()

        media = os.listdir("/media")
        media  = map(lambda x: "/media/"+x, media)

        mnt = os.listdir("/mnt")
        mnt  = map(lambda x: "/mnt/"+x, mnt)

        mount = commands.getoutput("mount")
        mount = mount.split("\n")


        for e in mount:
            for k in media:
                if e.find(k) != -1: devices.append(e)
        for e in mount:
            for k in mnt:
                if e.find(k) != -1: devices.append(e)

        pat = r"(.+) on (.+) type"
        for e in devices:
            match = re.findall(pat,e,flags=0)[0]
            self.devices[match[0]] = match[1] 

        for e in self.devices.keys():      
            size, info = self.getVolumeInfo(e)
            data = ' --> '.join([e,self.devices[e]])
            info = ' '.join( [info, ('/'.join(size)) ] ) 
            self.list2.addItem(' '.join( [data,info] ))

   

    def showMenuView(self):
        self.toolBtn.showMenu()

    def viewAll(self):
        self.notViewHiddenAction.setCheckable(False)
        self.viewOnlyDirsAction.setCheckable(False)
        self.viewAllAction.setCheckable(True)
        self.viewAllAction.setChecked(True)        
        self.model.setFilter(QtCore.QDir.Hidden | QtCore.QDir.Dirs \
            | QtCore.QDir.Files | QtCore.QDir.NoDotAndDotDot)
            
    def notViewHidden(self):
        self.notViewHiddenAction.setCheckable(True)
        self.notViewHiddenAction.setChecked(True)
        self.viewOnlyDirsAction.setCheckable(False)
        self.viewAllAction.setCheckable(False)
        self.model.setFilter(QtCore.QDir.Dirs | QtCore.QDir.Files \
            | QtCore.QDir.NoDotAndDotDot)

    def viewOnlyDirs(self):
        self.notViewHiddenAction.setCheckable(False)
        self.viewOnlyDirsAction.setCheckable(True)
        self.viewAllAction.setCheckable(False)
        self.viewOnlyDirsAction.setChecked(True)
        self.model.setFilter(QtCore.QDir.Dirs | QtCore.QDir.NoDotAndDotDot)

    def closeEvent(self, event):
        reply=QtGui.QMessageBox.question(self, decode(_('Message')), \
        decode(_("You really want to quit?")),QtGui.QMessageBox.Yes,QtGui.QMessageBox.No)
        if reply==QtGui.QMessageBox.Yes:
            self.stop()
            event.accept()
        else:
            event.ignore()


    #methods
    def addTo(self): 
        #files/folders
        if self.tabMode.currentIndex() == 0:
            paths = self.getPaths()
            if paths != []:
                for path in paths:
                    if self.paths.keys().count(uencode(path)) == 0:
                        self.paths[uencode(path)] = None
                        self.list.addItem(path)
        else:
        #devices
            indexes = [e.row() for e in self.list2.selectionModel().selectedIndexes()]
            tmp = [ (e, self.devices[e]) for i,e in enumerate(self.devices.keys()) if i in indexes ]

            for t in tmp:            
                if not self.freeSpace.has_key(t[0]):
                    self.freeSpace[t[0]] =  t[1]

                    text = t[0] + " ---> " + t[1]
                    self.list.addItem(text)            
      
#            print "freeSpace", self.freeSpace            
                    

    def removeFrom(self):
        if self.list.currentRow() != -1:
            item = self.list.takeItem(self.list.currentRow())
            text = uencode(item.text())
            try:
                text = text[:text.index("--------->")] 
            except: pass
            self.paths.pop(text)

        
    def setCommand(self):
        self.ledit.setText(self.shred.command)

    def setTextOfdd(self):
        self.textArea.append(decode("dd: creating file free space size ... "))

    def clearTo(self):
        self.paths = {}
        self.list.clear()

    def removeFromList(self, path):
        item = self.list.findItems(decode(path), QtCore.Qt.MatchContains)
        row = self.list.row(item[0])
        item = self.list.takeItem(row)

    def insertResult(self, path, result):
        if self.paths.has_key(path):
            self.paths[path] = result
        item = self.list.findItems(decode(path), QtCore.Qt.MatchContains)
        row = self.list.row(item[0])
        item = self.list.takeItem(row)

        if result == True: 
            self.list.insertItem(row, decode(path)+u"")	
            self.list.item(row).setForeground(Qt.darkGreen)
        else: 
            self.list.insertItem(row, decode(path)+u"")	
            self.list.item(row).setForeground(Qt.darkRed)
            

    def lockToolBar(self):
        self.homeAction.setEnabled(False)
        self.refreshAction.setEnabled(False)
        self.toolBtn.setEnabled(False)
        self.addAction.setEnabled(False)
        self.removeAction.setEnabled(False)
        self.clearAction.setEnabled(False)   
        self.SaSAction.setIcon(QIcon("/usr/share/shreg/stop.png"))

    def unlockToolBar(self):
        self.homeAction.setEnabled(True)
        self.refreshAction.setEnabled(True)
        self.toolBtn.setEnabled(True)
        self.addAction.setEnabled(True)
        self.removeAction.setEnabled(True)
        self.clearAction.setEnabled(True)       
        self.SaSAction.setIcon(QIcon("/usr/share/shreg/start.png"))
        self.SaSAction.setEnabled(True)

    def SaS(self):
         
        self.isStartNow = not(self.isStartNow)
        if self.isStartNow: self.delete()
        else: self.stop()


    def stop(self):
        self.SaSAction.setEnabled(False)
        if self.shred != None:
            self.shred.setEnd()
            if os.path.exists("/proc/%s"%(self.shred.pid)):
                print "shred programm is killed"
                os.kill(self.shred.pid, signal.SIGTERM) 
        self.refresh()       

    def delete(self):

        if (not self.paths.values().count(None)) and (not self.freeSpace.keys() != []):
            reply = QtGui.QMessageBox.question(self, decode(_('Message')),
                decode(_("The list is empty")), QtGui.QMessageBox.Yes)
            self.isStartNow = not(self.isStartNow)    
            return

        reply = QtGui.QMessageBox.question(self, decode(_('Message')),
            decode(_("Sure? Files on the list will be removed from your system.")),
            QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)

        if reply == QtGui.QMessageBox.Yes:

            self.lockToolBar()

            self.shred = ThreadShred(self, self.countSpBox.value())
            self.shred.start()
            self.textArea.setVisible(True)
            self.pbarAll.setVisible(True)
            self.pbarStep.setVisible(True)
            self.ledit.setVisible(True)
            self.pbarStep.setMaximum(0)
            while True:
                
                QApplication.processEvents()
             
                if self.shred.check():
                    break
                value,path,stdout = self.shred.getInfo()
                self.pbarAll.setValue(value)
                self.pbarAll.setFormat(decode(path))
                if stdout != '': self.textArea.append(decode(stdout))

                self.pbarAll.update()                
                self.pbarStep.update()

            
            self.pbarStep.setMaximum(1)
            self.pbarStep.setValue(1)
            self.textArea.setVisible(False)
            self.pbarStep.setVisible(False)
            self.ledit.setVisible(False)  
            self.pbarAll.setValue(100)  
            self.pbarAll.setFormat(decode(_("Ready")))
            self.refresh()

            self.ledit.setText("")
            
            self.unlockToolBar()
            self.shred = None
        else:
            self.isStartNow = not(self.isStartNow)

    def getPaths(self):        
        lst = self.tree.selectionModel().selectedIndexes();
        model = self.tree.model();
        row = -1
        result = []
        for e in lst:
            if (e.row() != row) and (e.column() == 0):
                fileInfo = model.fileInfo(e);
                result.append(fileInfo.filePath())

        panicResult = [str(p) for p in result if p in systemDirs]
        notPanicResult = [p for p in result if p not in systemDirs]        

        if panicResult != []:
            msg =  decode(_("Следующие каталоги:\n%s\nявляются системными и будут пропущены."%('\n'.join(panicResult))))
            reply=QtGui.QMessageBox.question(self, decode(_('Attention')), msg,
            QtGui.QMessageBox.Ok)        

        return notPanicResult

    def goHome(self):
        path = QtCore.QDir.homePath()
        if self.tree.currentIndex() != self.tree.setCurrentIndex(self.model.index(path)):
            self.tree.setCurrentIndex(self.model.index(path))
        self.tree.scrollTo(self.model.index(path))

    def refresh(self):
        self.model.refresh(self.list.rootIndex())
        self.updateDevices()

        self.freeSpace = {}
        self.paths = {}
        #self.list.clear()
        self.isStartNow = False        

    def showCommands(self):
        f = open("/tmp/shredgui_commands", 'a')
        for e in self.commands: f.write(e+'\n')
        f.close()
   
    def openLog(self):
        p = subprocess.Popen("mousepad /var/log/shred", shell=True)
        p.wait()
                
        

