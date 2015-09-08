from time import localtime
from time import strftime
from PyQt5.QtGui import QTextCursor

__author__ = 'Fabian Graf'


class Log(object):
    def __init__(self, ui):
        # save ui reference
        self.ui = ui

        # make logtext variable
        self.logtext = ""

    def new_log_message(self, text, color):
        # get localtime
        lt = localtime()

        # new logtext with blue time
        if color == "blue":
            self.logtext += "<b style=color:'#3DAEE9'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + text + "<br>"
        elif color == "yellow":
            self.logtext += "<b style=color:'#FDBC4B'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + text + "<br>"

        # set logtext to logView
        self.ui.logView.setHtml(self.logtext)

        # move cursor to bottom
        self.ui.logView.moveCursor(QTextCursor.End)
