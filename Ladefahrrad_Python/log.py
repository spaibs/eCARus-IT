from time import localtime
from time import strftime
from PyQt5.QtGui import QTextCursor

__author__ = 'ga38sor'


class Log(object):
    def __init__(self, ui):
        self.ui = ui
        self.logtext = ""

        self.ui.logView.setHtml("")

    def new_log_message(self, text):
        lt = localtime()
        print("new log message")
        logtext = self.ui.logView.toHtml()

        #logtext += "<b style=color:'#3F51B5'>" + str(strftime("%H:%M:%S", lt)) + "</style></b>: " + text + "<br>"

        logtext += text

        self.ui.logView.setHtml(logtext)

        self.ui.logView.moveCursor(QTextCursor.End)



