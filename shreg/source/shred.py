# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import time
import stat
import threading
from threading import Thread

from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import * 
from PyQt4.QtGui import * 

from conversion import *



class ThreadReadStd(Thread):
    def __init__(self,  stdout):
        Thread.__init__(self) 
        
        self.stdout = stdout        

        self.stop = False
        self.line = None
        self.lst = []

    def run(self):
        while True:
            self.line = self.stdout.readline()
            self.lst.append(self.line)

            if self.stop: break

        print "Stdout thread is end"

    def setEnd(self):
        self.stop = True

    def check(self):
        if len(self.lst) : return True
        else: return False

    def getInfo(self):
        return self.lst.pop(0)[:-1]

class ThreadShred(Thread):
    def __init__(self,  parent, count):
        Thread.__init__(self) 
        
        self.res = False
        self.parent = parent
        self.path = None
        self.value = None
        self.command = None
        self.pid = None
        self.stop = False
        self.shredIter = count

        self.stdout = None

    def shred(self, shredIter, path):
        self.command = "shred -n %s -u -v '%s'"%(shredIter, decode(path))      
        self.parent.emit(SIGNAL("setCommand"))
        p = subprocess.Popen(self.command,
            stderr=subprocess.PIPE,
            shell=True)

        self.stdout = ThreadReadStd(p.stderr)
        self.stdout.start()
        self.pid = p.pid
        p.wait() 
        self.stdout.setEnd()        
        
        if not self.stop: 
            open("/var/log/shred", 'a').write(" ".join([path, "done", "\n"]))
        elif path.find("shreg_file_to_delete") != -1:
            os.remove(path)
        #p.communicate()

    def runFiles(self):

        shredIter = self.shredIter
        step = 100 / self.parent.paths.values().count(None)

        for index, path in enumerate(self.parent.paths.keys()): 
            if self.stop: break

            self.path = path      
            if not self.parent.paths.values().count(None): continue

            value = step * (index+1)
            self.value = value
            
            #check0
            if not self.parent.paths[path] == None:
                continue                

            #check1
            if not os.path.exists(path):
                self.parent.insertResult(path, False)
                continue

            if os.path.isdir(path):
                if os.path.exists("/tmp/shreg_files"): os.remove("/tmp/shreg_files")
                self.command = "find '%s'/* -type f > /tmp/shreg_files"%(decode(path))
                self.parent.emit(SIGNAL("setCommand"))
              
                p = subprocess.Popen(self.command, shell=True)
                self.pid = p.pid
                p.wait()
                if self.stop: break
            
                f = open("/tmp/shreg_files", 'r')
                files = [line[:-1] for line in f.readlines()]
                f.close()

                for f in files:
                    self.shred(shredIter,f)
                    if self.stop: break
            
                if self.stop: break                            

                self.command = "rm -rf '%s'"%(decode(path))
                self.parent.emit(SIGNAL("setCommand"))

                p = subprocess.Popen(self.command, shell=True)
                self.pid = p.pid
                p.wait()
                if self.stop: break

            else:
                self.shred(shredIter,path)                
                if self.stop: break                

            self.parent.removeFromList(path)
            #self.parent.insertResult(path, True)  


    def free_volume(self, filename):
        stats = subprocess.Popen(["df", "-Pk", filename], stdout=subprocess.PIPE).communicate()[0]
        return str(int(stats.splitlines()[1].split()[3]) / 1024)


    def runDevices(self):

        shredIter = self.shredIter
        step = 100 / len(self.parent.freeSpace.values())


        for index, dev in enumerate(self.parent.freeSpace.keys()):
            if self.stop: break

            folder = self.parent.freeSpace[dev]             

            self.path = dev         
            value = step * (index+1)
            self.value = value

            if os.access(folder, os.W_OK):
                
                size =  self.free_volume(dev)       
                filename =  "'%s/shreg_file_to_delete'"%(decode(folder))        
                self.command = "dd if=/dev/zero of=%s bs=1048576 count=%s" %  (filename,size)
                self.parent.emit(SIGNAL("setCommand"))                
                self.parent.emit(SIGNAL("setTextOfdd"))

                p = subprocess.Popen(self.command, shell=True)
                self.pid = p.pid
                p.wait()

                if self.stop: break

                self.shred(shredIter, filename)   
                if self.stop: break                
                                                             

            self.parent.removeFromList(folder)
            
   

    def run(self): 

        if self.parent.paths.values().count(None) != 0:
            self.runFiles()

   
        if self.parent.freeSpace.keys() != []:
            self.runDevices()
                
        self.res = True
        print "Shred thread is end"


    def setEnd(self):
        self.stop = True

    def check(self):
        return self.res

    def getInfo(self):

        if self.stdout != None:
            if self.stdout.check():
                return self.value, self.path, self.stdout.getInfo()

        return self.value, self.path, ''
