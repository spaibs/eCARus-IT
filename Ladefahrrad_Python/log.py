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

    def new_log_message(self, text):
        # get localtime
        lt = localtime()

        # new logtext with blue time
        self.logtext += "<b style=color:'#3F51B5'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + text + "<br>"

        # set logtext to logView
        self.ui.logView.setHtml(self.logtext)

        # move cursor to bottom
        self.ui.logView.moveCursor(QTextCursor.End)
