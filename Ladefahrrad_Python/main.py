import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.uic import *

# import Sender
from sender import Sender

__author__ = 'Fabian Graf'


class Main(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        # load gui.ui
        self.ui = loadUi('gui/gui.ui')

        # show window
        self.ui.show()

        # make new sender object
        self.sender = Sender(self.ui)

        # set events
        self.set_events()

    def set_events(self):
        # sendButton clicked
        self.ui.sendButton.clicked.connect(self.sender.send)

        # messageInput return pressed
        self.ui.messageInput.returnPressed.connect(self.sender.send)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    # make new window
    window = Main()

    sys.exit(app.exec_())
