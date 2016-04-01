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
        # make empty message
        message = ""

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

        elif type == "checkbox-automode":
            # get state
            state = self.ui.automodeCheckbox.isChecked()

            # is checked?
            if state:
                message = "g0300000001"
            else:
                message = "g0300000000"

        # lamps
        elif type == "checkbox-lamp":
            # get state
            state_l1 = self.ui.lamp1Checkbox.isChecked()
            state_l2 = self.ui.lamp2Checkbox.isChecked()
            state_l3 = self.ui.lamp3Checkbox.isChecked()
            state_l4 = self.ui.lamp4Checkbox.isChecked()

            state_all = "g040000" + str(int(state_l1)) + str(int(state_l2)) + str(int(state_l3)) + str(int(state_l4))

            print(state_all)

            # send
            self.sock.sendto(state_all.encode('utf-8'), (self.ip, self.port))

            # new log message
            self.log.new_log_message(state_all + " sent", "blue")

        if message != "":
            # send
            self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

            # new log message
            self.log.new_log_message(message + " sent", "blue")

    def reset(self):
        self.ui.voltageSlider.setValue(0)
        self.ui.currentSlider.setValue(0)

        # set checkboxes
        self.ui.automodeCheckbox.setChecked(True)
        self.ui.lamp1Checkbox.setChecked(True)
        self.ui.lamp2Checkbox.setChecked(True)
        self.ui.lamp3Checkbox.setChecked(True)
        self.ui.lamp4Checkbox.setChecked(True)

        # set voltage message
        message = "g0100000000"

        # send
        self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

        # set voltage message
        message = "g0200000000"

        # send
        self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

        # set automode message
        message = "g0300000001"

        # send
        self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

        # set lamps message
        message = "g0400001111"

        # send
        self.sock.sendto(message.encode('utf-8'), (self.ip, self.port))

        # new log message
        self.log.new_log_message("reset", "yellow")
