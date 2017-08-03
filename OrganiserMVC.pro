#-------------------------------------------------
#
# Project created by QtCreator 2017-06-08T21:32:01
#
#-------------------------------------------------

QT       += core gui sql
CONFIG   += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OrganiserMVC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    date.cpp \
    task.cpp \
    iconmanager.cpp \
    db.cpp \
    datecontrollers.cpp \
    qcustomlabel.cpp \
    customshadoweffect.cpp \
    taskprogressbar.cpp \
    iconbutton.cpp \
    taskdialog.cpp

HEADERS  += mainwindow.h \
    date.h \
    task.h \
    constants.h \
    iconmanager.h \
    db.h \
    datecontrollers.h \
    qcustomlabel.h \
    customshadoweffect.h \
    taskprogressbar.h \
    iconbutton.h \
    taskdialog.h

FORMS    += mainwindow.ui \
    taskdialog.ui

RESOURCES += \
    resources.qrc
