# -*- coding: utf-8 -*-

import sys
reload(sys)
sys.setdefaultencoding('cp932')
tes = sys.getdefaultencoding()

import os
import cv2
import numpy as np
import pyws as m
import winxpgui

from PIL import ImageGrab
from PyQt4 import QtGui, QtCore

from datetime import datetime
import ConfigParser

class ControllerBoxWidget(QtGui.QWidget):
    def __init__(self, config, parent=None):
        QtGui.QWidget.__init__(self, parent=parent)
        self.setup(config)

    def setup(self,config):
        self.start = QtGui.QPushButton('Start', parent=self)
        self.stop = QtGui.QPushButton('Stop', parent=self)
        self.quit = QtGui.QPushButton('Quit', parent=self)

        self.intervalLavel = QtGui.QLabel('Interval(msec)',parent=self)
        self.intervalLavel.setAlignment(QtCore.Qt.AlignRight)
        self.interval = QtGui.QLineEdit(parent=self)
        self.interval.setValidator(QtGui.QIntValidator())
        self.interval.setText(config.get('config', 'interval'))

        self.windowTitleLavel = QtGui.QLabel('Window Title',parent=self)
        self.windowTitleLavel.setAlignment(QtCore.Qt.AlignRight)
        self.windowTitle = QtGui.QLineEdit(parent=self)
        self.windowTitle.setText(config.get('config', 'title'))


        layout = QtGui. QGridLayout()
        layout.addWidget(self.start, 0,0)
        layout.addWidget(self.stop, 0,1)
        layout.addWidget(self.intervalLavel, 1,0)
        layout.addWidget(self.interval, 1,1)
        layout.addWidget(self.windowTitleLavel, 2,0)
        layout.addWidget(self.windowTitle, 2,1)
        layout.addWidget(self.quit, 3,1)

        self.setLayout(layout)

    def locInput(self):
        self.interval.setReadOnly(True)
        self.windowTitle.setReadOnly(True)

    def releaseInput(self):
        self.interval.setReadOnly(False)
        self.windowTitle.setReadOnly(False)

# 画面生成
class CaptureWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent=parent)
        self.setup()

    def setup(self):
        self.interval = 60000

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.getCapture)

        self.message = QtGui.QLabel('Setup')

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.message)
        self.setLayout(layout)

    def getCapture(self):
        directory = 'capture'
        if not os.path.isdir(directory):
            os.makedirs(directory)
        try :
            handle = m.getid(self.windowTitle)
            rect = winxpgui.GetWindowRect(handle)
        except IndexError as e:
            self.setMessage(str(e))
            return
        cpimg = ImageGrab.grab(rect)
        cpimg = np.asarray(cpimg)
        cpimg = cv2.cvtColor(cpimg, cv2.COLOR_RGB2BGR)
        cv2.imwrite(directory + '/' + datetime.now().strftime("%Y%m%d%H%M%S") + '.jpg', cpimg)

    def start(self, windowTitle, interval):
        try:
            self.setInterval(interval)
            self.setWindowTitle(windowTitle)
            self.timer.start()
            self.setMessage('Running...')
        except ValueError as e:
            self.setMessage(str(e))

    def stop(self):
        self.timer.stop()
        self.setMessage('Stopped')

    def setInterval(self, interval):
        self.interval = int(interval)
        self.timer.setInterval(self.interval)

    def setWindowTitle(self, string):
        self.windowTitle = str(string)

    def setMessage(self, str):
        self.message.setText(str)


def saveConfig(config, controller):
    config.set('config', 'interval', controller.interval.text())
    config.set('config', 'title', controller.windowTitle.text().toStdString())
    with open('config.ini', 'wb') as configfile:
        config.write(configfile)
    pass


## main

config = ConfigParser.RawConfigParser({'interval': '1000', 'title':''})
config.read('config.ini')

if not config.has_section('config'):
    config.add_section('config')



app = QtGui.QApplication(sys.argv)

panel = QtGui.QWidget()
panel_layout = QtGui.QVBoxLayout()
capture = CaptureWidget(panel)
controller = ControllerBoxWidget(config, panel)


controller.start.clicked.connect(controller.locInput)
controller.start.clicked.connect(lambda: capture.start(controller.windowTitle.text(), controller.interval.text()))

controller.stop.clicked.connect(capture.stop)
controller.stop.clicked.connect(controller.releaseInput)

controller.quit.clicked.connect(sys.exit)


panel_layout.addWidget(capture)
panel_layout.addWidget(controller)

panel.setLayout(panel_layout)


mw = QtGui.QMainWindow()
mw.setWindowTitle('App Capture')
mw.setCentralWidget(panel)
mw.show()
app.aboutToQuit.connect(app.deleteLater)
app.aboutToQuit.connect(lambda: saveConfig(config, controller))
app.exec_()
