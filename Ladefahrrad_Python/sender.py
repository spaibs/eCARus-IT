import change
import socket

__author__ = 'Fabian Graf'


class Sender(object):
    def __init__(self, ui, log):
        print("init")
        self.ui = ui
        self.log = log

        # make socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # define ip and port number
        self.ip = "10.162.70.132"
        self.port = 32701

    def send(self):
        # get values
        value = self.ui.messageInput.text()
        id = self.ui.idChooser.currentText()

        # change id to length 8
        value = change.to_8_chars(value)

        # create message
        message = id + value

        # send
        self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

        # new log message
        self.log.new_log_message("sending " + message)
