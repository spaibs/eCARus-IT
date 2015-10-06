#-------------------------------------------------
#
# Project created by QtCreator 2014-09-18T15:38:26
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT      += webkit


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

CONFIG   -= console

TARGET = Telemetrie
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    interpretethread.cpp \
    configuration.cpp \
    remotethread.cpp \
    udpthread.cpp \
    utility.cpp \
    #secdialog.cpp \
    helpwindow.cpp \
    periodicsendworker.cpp \
    mouse_label.cpp \
    special_label.cpp


HEADERS  += mainwindow.h \
    XInput.h \
    interpretethread.h \
    configuration.h \
    datatypes.h \
    remotethread.h \
    udpthread.h \
    utility.h \
    #secdialog.h \
    helpwindow.h \
    periodicsendworker.h \
    mouse_label.h \
    special_label.h

#INCLUDEPATH += xinput9_1_0.dll
INCLUDEPATH += .
win32: LIBS += -lXInput

FORMS    += mainwindow.ui \
    #secdialog.ui \
    helpwindow.ui

RESOURCES += \
    source.qrc

OTHER_FILES += \
    xinput9_1_0.dll \
    xinput9_1_0_32.dll

