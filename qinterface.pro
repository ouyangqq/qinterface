#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T12:54:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = ouyang1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    General_code/protocol.c \
    General_code/Process.c \
    General_code/kalman.c \
    General_code/Common.c \
    qextserial/qextserialport.cpp \
    logindlg.cpp \
    qcustomplot.cpp \
    skinwidget.cpp \
    myinputpanel.cpp \
    myinputpanelcontext.cpp

HEADERS  += \
    mainwindow.h \
    General_code/protocol.h \
    General_code/Process.h \
    General_code/kalman.h \
    General_code/General.h \
    General_code/Common.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport.h \
    logindlg.h \
    qcustomplot.h \
    skinwidget.h \
    myinputpanel.h \
    myinputpanelcontext.h

win32 {
     SOURCES += qextserial/qextserialport_win.cpp
}

unix {
     SOURCES += qextserial/qextserialport_unix.cpp
}

FORMS    += mainwindow.ui \
    logindlg.ui \
    myinputpanelform.ui

RESOURCES += \
    res.qrc \
    qw.qrc
