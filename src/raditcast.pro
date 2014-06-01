#-------------------------------------------------
#
# Raditcast - Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui

TARGET = raditcast
TEMPLATE = app
CONFIG += release

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui



# -------------------------------------------------


unix {

LIBS += /usr/lib/libbass.so \
        /usr/lib/libbassenc.so \
        /usr/lib/libqvumeterplug.so


INCLUDEPATH = ../lib/bass/include \
              ../lib/bassenc/include \
              ../lib/vumeter/include


OBJECTS_DIR =../out
MOC_DIR = ../out
UI_DIR = ../out
DESTDIR = ../LinuxDesktop

}

OTHER_FILES +=

RESOURCES += \
    radit.qrc

RC_FILE = radit.rc

