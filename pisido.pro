#-------------------------------------------------
#
# Project created by QtCreator 2011-04-10T13:32:53
#
#-------------------------------------------------

QT       += core gui

TARGET = pisido
TEMPLATE = app

VERSION = 1.0.0
DEFINES += PISIDO_VERSION=\\\"$$VERSION\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    configurationdialog.cpp

HEADERS  += mainwindow.h \
    configurationdialog.h

FORMS    += mainwindow.ui \
    configurationdialog.ui

OTHER_FILES += \
    pisido_tr.qm \
    pisido_tr.ts

RESOURCES += \
    pisido.qrc
