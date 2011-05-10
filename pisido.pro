#-------------------------------------------------
#
# Project created by QtCreator 2011-04-10T13:32:53
#
#-------------------------------------------------

QT       += core gui xml network

TARGET = pisido
TEMPLATE = app

VERSION = 1.1.0
DEFINES += PISIDO_VERSION=\\\"$$VERSION\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    configurationdialog.cpp \
    helpdialog.cpp \
    pspec_base.cpp \
    pspec_package.cpp \
    pspec_pisi.cpp \
    pspec_source.cpp \
    pspec_update.cpp

HEADERS  += mainwindow.h \
    configurationdialog.h \
    helpdialog.h \
    pspec_update.h \
    pspec_source.h \
    pspec_pisi.h \
    pspec_package.h \
    pspec_base.h

FORMS    += mainwindow.ui \
    configurationdialog.ui \
    helpdialog.ui

OTHER_FILES += \
    pisido_tr.qm \
    pisido_tr.ts \
    README \
    OKUBUNU \
    LICENSE \
    LISANS

RESOURCES += \
    pisido.qrc
