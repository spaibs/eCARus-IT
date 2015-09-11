from time import localtime
from time import strftime
from PyQt5.QtGui import QTextCursor

__author__ = 'Fabian Graf'


class Log(object):
    def __init__(self, ui):
        # save ui reference
        self.ui = ui

        # make hold variable
        self.hold = False

        # make logtext variable
        self.logtext = ""

    def new_log_message(self, text, color):
        if not self.hold:
            # get localtime
            lt = localtime()

            # new logtext with blue time
            if color == "blue":
                self.logtext += "<b style=color:'#3DAEE9'>"
            elif color == "yellow":
                self.logtext += "<b style=color:'#FDBC4B'>"
            elif color == "green":
                self.logtext += "<b style=color:'#1CDC9A'>"

            # add actual text + end
            self.logtext += str(strftime("%H:%M:%S", lt)) + "</style></b>: " + text + "<br>"

            # set logtext to logView
            self.ui.logView.setHtml(self.logtext)

            # move cursor to bottom
            self.ui.logView.moveCursor(QTextCursor.End)

    def toggle_hold(self):
        # toggle hold variable
        if self.hold:
            self.hold = False

            # change holdButton's text
            self.ui.holdButton.setText("Anhalten")

            # new log message
            self.new_log_message("log continued", "yellow")
        else:
            # new log message
            self.new_log_message("log stopped", "yellow")

            self.hold = True

            # change holdButton's text
            self.ui.holdButton.setText("Fortsetzen")

    def clear_log(self):
        # set logtext ""
        self.logtext = ""

        # set logView
        self.ui.logView.setPlainText(self.logtext)
