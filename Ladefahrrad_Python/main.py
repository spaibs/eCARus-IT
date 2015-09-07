import sys
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow
from PyQt5.uic import *

__author__ = 'Fabian Graf'


class Main(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.ui = loadUi('X:/projects/2010/python/pyqt_tutorial/pyqt_tutorial.ui')
        self.ui.show()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Main()
    window.show()
    sys.exit(app.exec_())