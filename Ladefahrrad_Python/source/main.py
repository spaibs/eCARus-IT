import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.uic import *

# import Sender
from sender import Sender

# import Log
from log import Log

__author__ = 'Fabian Graf'


class Main(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        # load gui.ui
        self.ui = loadUi('../gui/gui.ui')

        # show window
        self.ui.show()

        # make new log object
        self.log = Log(self.ui)

        # make new sender object
        self.sender = Sender(self.ui, self.log)

        # set events
        self.set_events()

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
        self.ui.voltageSlider.valueChanged.connect(lambda: self.sender.send("slider-voltage"))
        self.ui.currentSlider.valueChanged.connect(lambda: self.sender.send("slider-current"))

        # reset action
        self.ui.resetActionButton.triggered.connect(self.sender.reset)

if __name__ == "__main__":
    # new QApplication
    app = QApplication(sys.argv)

    # make new window
    window = Main()

    sys.exit(app.exec_())
