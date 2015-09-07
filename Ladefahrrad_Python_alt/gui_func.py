__author__ = "Fabian Graf"

"""
functions for GUI:
- reset()
- show_help()

reset(): Resets following components:
- voltage
- current
- toggles all lamps off

show_help(): Shows help window.
"""

from PyQt5.uic import *


def reset(window, sock, ip, port):
    # Turn Checkboxes off
    window.lamp1.setChecked(False)
    window.lamp2.setChecked(False)
    window.lamp3.setChecked(False)
    window.lamp4.setChecked(False)

    # Turn Buttons on GUI off
    sock.sendto("g0400000000".encode('utf-8'), (ip, port))

    # Turn voltage off
    sock.sendto("g0100000000".encode('utf-8'), (ip, port))

    # Turn current off
    sock.sendto("g0200000000".encode('utf-8'), (ip, port))

    print("GUI reset.")


def show_help():
    print("Help opened.")

    help_window = loadUi("hilfe_gui.ui")
    help_window.exec_()
