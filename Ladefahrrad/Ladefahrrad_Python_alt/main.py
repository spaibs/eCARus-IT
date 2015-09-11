from msilib import Dialog
import time
from threading import Thread
from time import *
import time
__author__ = 'Fabian Graf'
__appname__ = "Ladefahrrad"

import sys
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import QApplication, QMainWindow, QDialog, QTreeWidgetItem, QTreeView, QTreeWidget, QMessageBox, QErrorMessage
from PyQt5 import Qt
from PyQt5 import *
import signal
from PyQt5.uic import *

import socket
import gui_func

from receiver import Receiver


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
ip = "10.162.70.132"
port = 32701

#127 when on EBit
ip_rec = "0.0.0.0"
port_rec = 40001

t = False

logText = ""

l1 = False
l2 = False
l3 = False
l4 = False
spannung = 0
strom = 0

hold = False
logText = ""


app = QApplication(sys.argv)
w = loadUi("gui.ui")
ei = loadUi("einstellungen_gui.ui")



# Items for tree
voltageItem = QTreeWidgetItem(["Spannung", str(spannung) + " V"])
w.tree.addTopLevelItem(voltageItem)

currentItem = QTreeWidgetItem(["Strom", str(strom) + " A"])
w.tree.addTopLevelItem(currentItem)

wattItem = QTreeWidgetItem(["Watt", "0 W"])
w.tree.addTopLevelItem(wattItem)








def newLogMessage(lm):
    lt = localtime()
   # if hold == False:
        #w.logView.insertPlainText(str(strftime("%H:%M:%S", lt)) + ": " + lm + "\n")
        #w.logView.insertPlainText("fasjgfsjagdfhsadfjskadgjh\n")
        #global logtext
        #logtext = "<b style=color:'#3F51B5'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + lm + "<br>"
        #w.logView.insertHtml("<b style=color:'#3F51B5'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + lm + "<br>")
    #time.sleep(0.1)
    #w.logView.moveCursor(QTextCursor.End)


def change():
    ei = loadUi("einstellungen_gui.ui")
    r = ei.exec_()
    if r == QDialog.Accepted:
        ip = ei.ip.text()
        port = ei.port.text()
        print(ip)


def senden():
    sock.sendto(w.msg.text().encode('utf-8'), (ip, port))

def send(msg):
    sock.sendto(msg.encode('utf-8'), (ip, port))
    w.msg.setText(msg)

def edit():
    print("edit..")

def make8(c):
    c=str(c)
    global spannung
    spannung = float(c)
    voltageItem.setData(1,0,str("{0:.2f}".format(float(spannung)/1000)) + " V")
    wattItem.setData(1,0,str("{0:.2f}".format(float(spannung)/1000 * float(strom)/1000)) + " W")
    if (len(c) < 8):
        for i in range(0,8-len(c)):
            c = "0" + c
    msg = "g01" + c
    send(msg)



def make82(c):
    c = str(c)
    global strom
    strom = float(c)
    currentItem.setData(1,0,str("{0:.2f}".format(float(strom)/1000)) + " A")
    wattItem.setData(1,0,str("{0:.2f}".format(float(spannung)/1000 * float(strom)/1000)) + " W")
    if len(c) < 8:
        for i in range(0,8-len(c)):
            c = "0" + c
    msg = "g02" + c
    send(msg)


def updateSend(msg, p):
    sock.sendto((p + msg).encode('utf-8'), (ip, port))
    print(p + msg)

def makeTo8(c):
    c = str(c)
    if len(c) < 8:
        for i in range(0,8-len(c)):
            c = "0" + c
    return c
def demostart():
    w.actionDemo.setEnabled(False)
    w.actionDemoBeenden.setEnabled(True)
    w.actionSenden.setEnabled(False)
    w.msg.setEnabled(False)
    w.senden.setEnabled(False)
    w.demo_s.setEnabled(False)
def demostop():
    w.actionDemo.setEnabled(True)
    w.actionDemoBeenden.setEnabled(False)
    w.actionSenden.setEnabled(True)
    w.msg.setEnabled(True)
    w.senden.setEnabled(True)
    w.demo_s.setEnabled(True)
def spng():
    spng = w.spngSlider.value()
    make8(spng)
def stro():
    stro = w.strSlider.value()
    make82(stro)

def update(a):
    print(a)
    updateSend(makeTo8(a), "g03")

def updatelamp():
    m = ""
    global l1
    global l2
    global l3
    global l4

    if (l4 == False):
        m = "1" + m
        w.lamp4.setChecked(True)
    else:
        m = "0" + m
        w.lamp4.setChecked(False)
    if (l3 == False):
        m = "1" + m
        w.lamp3.setChecked(True)
    else:
        m = "0" + m
        w.lamp3.setChecked(False)
    if (l2 == False):
        m = "1" + m
        w.lamp2.setChecked(True)
    else:
        m = "0" + m
        w.lamp2.setChecked(False)
    if (l1 == False):
        m = "1" + m
        w.lamp1.setChecked(True)
    else:
        m = "0" + m
        w.lamp1.setChecked(False)
    updateSend("0000" + m, "g04")
    newLogMessage("Lampen aktualisiert: " + "g040000" + m)


def automodeToggle():
    global t
    if (t == True):
        update("1")
        newLogMessage("g0300000001: Automode: " + str(t))
        t = False
    elif (t == False):
        update("0")
        newLogMessage("g0300000000: Automode: " + str(t))
        t = True




def lamp1():
    global l1
    if (l1 == True):
        l1 = False
    elif (l1 == False):
        l1 = True
    updatelamp()

def lamp2():
    global l2
    if (l2 == True):
        l2 = False
    elif (l2 == False):
        l2 = True
    updatelamp()


def lamp3():
    global l3
    if (l3 == True):
        l3 = False
    elif (l3 == False):
        l3 = True
    updatelamp()


def lamp4():
    global l4
    if (l4 == True):
        l4 = False
    elif (l4 == False):
        l4 = True
    updatelamp()





def empfangen():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    s.bind((ip_rec, port_rec))

    while True:
        data, addr = s.recvfrom(1024)

        #newLogMessage("Nachricht empfangen: " + str(data)[2:-1])
        print("Nachricht empfangen: " + str(data)[2:-1])

        #time.sleep(0.5)

        global l1
        global l2
        global l3
        global l4

        id = str(data)[3:5]
        if id == "02":
            la1 = str(data)[9:10]
            la2 = str(data)[10:11]
            la3 = str(data)[11:12]
            la4 = str(data)[12:13]

            if la1 == "1":
                l1 = False
            else:
                l1 = True

            if la2 == "1":
                l2 = False
            else:
                l2 = True

            if la3 == "1":
                l3 = False
            else:
                l3 = True

            if la4 == "1":
                l4 = False
            else:
                l4 = True
            newLogMessage("Nachricht interpretiert als: Lampen ein/aus")
            #time.sleep(0.5)
            updatelamp()





#wer = Thread(target=empfangen)
#wer.start()



def ulog():
    while True:
        print("log u")
        #w.logView.setHtml("")
        #buf = QString('<html><body>jkj</body></html>'.decode('utf-8'))
        buf = str("dsa")
        #w.logView.setHtml(buf)
        #w.logView.insertHtml("da")
        sleep(3)

#ulo = Thread(target=ulog)
#ulo.start()

def holdt():
    global hold
    if not hold:
        hold = True
        w.holdButton.setText("Fortsetzen")
    else:
        hold = False
        w.holdButton.setText("Anhalten")

def clearLog():
    print("Log cleared.")
    w.logView.setText("")
    global logText
    logText = ""


def new_log_message(text):
    print("setting log..")
    lt = localtime()
    if hold == False:
        global logText

        text2 = "<b style=color:'#3F51B5'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + str(text) + "<br>"
        #text2 = "<b style=color:'#3F51B5'>"
        print("fertig")
        logText = logText + text2

        w.logView.setHtml(logText)
        w.logView.moveCursor(QTextCursor.End)


    #time.sleep(0.1)



# Receiver Thread
receiver = Receiver()
receiver.data_received.connect(new_log_message)
receiver.start()

def shutdown():
    print("bye..")
    #wer.exit()
    receiver.exit()
    #ulo.exit()

# Buttons
w.senden.clicked.connect(senden)
w.demo_s.clicked.connect(demostart)
w.clearButton.clicked.connect(clearLog)
w.holdButton.clicked.connect(holdt)

# Checkboxes
w.lamp1.clicked.connect(lamp1)
w.lamp2.clicked.connect(lamp2)
w.lamp3.clicked.connect(lamp3)
w.lamp4.clicked.connect(lamp4)

# Main Window
w.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)
w.destroyed.connect(shutdown)

# Toolbar
w.actionEinstellungen.triggered.connect(change)
w.actionDemo.triggered.connect(demostart)
w.actionDemoBeenden.triggered.connect(demostop)
w.actionSenden.triggered.connect(senden)
w.actionHilfe.triggered.connect(gui_func.show_help)
w.actionLampe_Senden.triggered.connect(updatelamp)
w.actionReset.triggered.connect(lambda: gui_func.reset(w, sock, ip, port))

# Sliders
w.spngSlider.valueChanged.connect(spng)
w.strSlider.valueChanged.connect(stro)

# LineEdit
w.autoModeCheckbox.stateChanged.connect(automodeToggle)

w.show()




sys.exit(app.exec_())