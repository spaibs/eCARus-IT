__author__ = 'Fabian Graf'


class Interpreter(object):
    def __init__(self, log, ui):
        # save log
        self.log = log

        # save ui
        self.ui = ui

    def interpret(self, text):
        self.log.new_log_message(text + " received", "green")
