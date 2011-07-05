#-------------------------------------------------
#
# Project created by QtCreator 2011-04-10T13:32:53
#
#-------------------------------------------------

QT       += core gui xml network

TARGET = pisido
TEMPLATE = app

VERSION = 2.0.0
DEFINES += PISIDO_VERSION=\\\"$$VERSION\\\"
LIBS += -lqscintilla2

# program will scan this dir for *.qm translation files.
# each file name must be like this : pisido_xx_XX.qm
# xx must be a valid language_COUNTRY pair, see QLocale doc.
# for example : pisido_tr_TR.qm , pisido_fr_FR.qm , ...
DEFINES += PISIDO_LANG_DIR=\\\"/home/huseyinkozan/calismalar/pisido/resource/translation/\\\"
#

SOURCES += main.cpp\
    mainwindow.cpp \
    configurationdialog.cpp \
    helpdialog.cpp \
    pisispbase.cpp \
    pisi.cpp \
    pisipackage.cpp \
    pisisource.cpp \
    pisiupdate.cpp \
    addupdatedialog.cpp \
    languagedialog.cpp \
    workspacedialog.cpp \
    consolewidget.cpp

HEADERS  += mainwindow.h \
    configurationdialog.h \
    helpdialog.h \
    pisispbase.h \
    pisi.h \
    pisipackage.h \
    pisisource.h \
    pisiupdate.h \
    addupdatedialog.h \
    languagedialog.h \
    workspacedialog.h \
    consolewidget.h

FORMS    += mainwindow.ui \
    configurationdialog.ui \
    helpdialog.ui \
    addupdatedialog.ui \
    languagedialog.ui \
    workspacedialog.ui \
    consolewidget.ui

OTHER_FILES += \
    README \
    OKUBUNU \
    LICENSE \
    LISANS \
    TODO \
    YAPILACAKLAR

RESOURCES += \
    pisido.qrc
