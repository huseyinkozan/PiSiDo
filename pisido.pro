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
INCLUDEPATH += ./h

# program will scan this dir for *.qm translation files.
# each file name must be like this : pisido_xx_XX.qm
# xx must be a valid language_COUNTRY pair, see QLocale doc.
# for example : pisido_tr_TR.qm , pisido_fr_FR.qm , ...
DEFINES += PISIDO_LANG_DIR=\\\"/home/huseyinkozan/calismalar/pisido/resource/translation/\\\"
#

SOURCES += cpp/main.cpp\
    cpp/application.cpp \
    cpp/mainwindow.cpp \
    cpp/configurationdialog.cpp \
    cpp/helpdialog.cpp \
    cpp/pisispbase.cpp \
    cpp/pisi.cpp \
    cpp/pisipackage.cpp \
    cpp/pisisource.cpp \
    cpp/pisiupdate.cpp \
    cpp/addupdatedialog.cpp \
    cpp/languagedialog.cpp \
    cpp/workspacedialog.cpp \
    cpp/consolewidget.cpp \
    cpp/aditionalfiledialog.cpp \
    addinstallfilelabeldialog.cpp

HEADERS  += h/mainwindow.h \
    h/application.h \
    h/configurationdialog.h \
    h/helpdialog.h \
    h/pisispbase.h \
    h/pisi.h \
    h/pisipackage.h \
    h/pisisource.h \
    h/pisiupdate.h \
    h/addupdatedialog.h \
    h/languagedialog.h \
    h/workspacedialog.h \
    h/consolewidget.h \
    h/aditionalfiledialog.h \
    addinstallfilelabeldialog.h

FORMS    += ui/mainwindow.ui \
    ui/configurationdialog.ui \
    ui/helpdialog.ui \
    ui/addupdatedialog.ui \
    ui/languagedialog.ui \
    ui/workspacedialog.ui \
    ui/consolewidget.ui \
    ui/aditionalfiledialog.ui \
    addinstallfilelabeldialog.ui

OTHER_FILES += \
    README \
    OKUBUNU \
    LICENSE \
    LISANS \
    TODO \
    YAPILACAKLAR

RESOURCES += \
    resource/pisido.qrc
