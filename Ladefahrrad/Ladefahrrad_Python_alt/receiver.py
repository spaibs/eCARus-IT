__author__ = 'Fabian Graf'

from PyQt5 import QtCore
import socket

# IP of this PC
ip = "0.0.0.0"

# Receive port
port = 40001

# new socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


class Receiver(QtCore.QThread):
    # Data received Signal
    data_received = QtCore.pyqtSignal(object)

    def __init__(self):
        QtCore.QThread.__init__(self)

        # bind Socket
        sock.bind((ip, port))

    def run(self):
        while True:
            # receive Data
            data, addr = sock.recvfrom(1024)

            # call new_log_message()
            self.data_received.emit("Nachricht empfangen: " + str(data)[2:-1])
