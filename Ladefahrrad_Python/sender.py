__author__ = 'ga38sor'


class Sender(object):
    def __init__(self, ui):
        print("init")
        self.ui = ui

    def send(self):
        value = self.ui.messageInput.text()
        id = self.ui.idChooser.currentText()
        print("-" + str(id))
