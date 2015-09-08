import change
import socket

__author__ = 'Fabian Graf'


class Sender(object):
    def __init__(self, ui, log):
        self.ui = ui
        self.log = log

        # make socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # create new info message
        self.log.new_log_message("socket created", "yellow")

        # define ip and port number
        self.ip = "10.162.70.132"
        self.port = 32701

    def send(self, type):
        if type == "debug":
            # get values
            value = self.ui.messageInput.text()
            id = self.ui.idChooser.currentText()

            # change id to length 8
            value = change.to_8_chars(value)

            # create message
            message = id + value
        elif type == "slider-voltage":
            voltage = str(self.ui.voltageSlider.value())

            # make 8 chars long
            voltage = change.to_8_chars(voltage)

            # make message
            message = "g01" + voltage

        elif type == "slider-current":
            current = str(self.ui.currentSlider.value())

            # make 8 chars long
            current = change.to_8_chars(current)

            # make message
            message = "g02" + current

        elif type == "reset":
            self.ui.voltageSlider.setValue(0)
            self.ui.currentSlider.setValue(0)
            message = ""

        if message != "":
            # send
            self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

            # new log message
            self.log.new_log_message("sending " + message, "blue")
