import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.uic import *
from PyQt5 import QtCore

# import Sender
from sender import Sender

# import Log
from log import Log

# import Listener
from listener import Listener

# import Interpreter
from interpreter import Interpreter

__author__ = 'Fabian Graf'


class Main(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        # load gui.ui
        self.ui = loadUi('../gui/gui.ui')

        self.ui.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)
        self.ui.destroyed.connect(self.shutdown)

        # show window
        self.ui.show()

        # make new log object
        self.log = Log(self.ui)

        # make new sender object
        self.sender = Sender(self.ui, self.log)

        # set events
        self.set_events()

        # initial reset
        self.sender.reset()

        # new listener
        self.listener = Listener()

        # new_data_received signal
        self.listener.data_received.connect(self.new_data_received)

        # start listener
        self.listener.start()

        # new interpreter
        self.interpreter = Interpreter(self.log, self.ui)

    def set_events(self):
        # sendButton clicked
        self.ui.sendButton.clicked.connect(lambda: self.sender.send("debug"))

        # holdButton clicked
        self.ui.holdButton.clicked.connect(self.log.toggle_hold)

        # clearButton clicked
        self.ui.clearButton.clicked.connect(self.log.clear_log)

        # messageInput return pressed
        self.ui.messageInput.returnPressed.connect(lambda: self.sender.send("debug"))

        # sliders value changed
        self.ui.voltageSlider.sliderReleased.connect(lambda: self.sender.send("slider-voltage"))
        self.ui.currentSlider.sliderReleased.connect(lambda: self.sender.send("slider-current"))

        # reset action
        self.ui.resetActionButton.triggered.connect(self.sender.reset)

        # automode checkbox
        self.ui.automodeCheckbox.stateChanged.connect(lambda: self.sender.send("checkbox-automode"))

        # lamp checkboxes
        self.ui.lamp1Checkbox.stateChanged.connect(lambda: self.sender.send("checkbox-lamp"))
        self.ui.lamp2Checkbox.stateChanged.connect(lambda: self.sender.send("checkbox-lamp"))
        self.ui.lamp3Checkbox.stateChanged.connect(lambda: self.sender.send("checkbox-lamp"))
        self.ui.lamp4Checkbox.stateChanged.connect(lambda: self.sender.send("checkbox-lamp"))

    def new_data_received(self, text):
        self.interpreter.interpret(text)

    def shutdown(self):
        self.listener.exit()

if __name__ == "__main__":
    # new QApplication
    app = QApplication(sys.argv)

    # make new window
    window = Main()

    sys.exit(app.exec_())
