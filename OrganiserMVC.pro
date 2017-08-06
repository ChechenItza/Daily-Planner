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


SOURCES += \
    controllers/datecontrollers.cpp \
    controllers/db.cpp \
    custom_widgets/customshadoweffect.cpp \
    custom_widgets/iconbutton.cpp \
    custom_widgets/qcustomlabel.cpp \
    custom_widgets/taskprogressbar.cpp \
    helpers/iconmanager.cpp \
    models/date.cpp \
    models/task.cpp \
    views/mainwindow.cpp \
    views/taskdialog.cpp \
    main.cpp

HEADERS  += \
    controllers/datecontrollers.h \
    controllers/db.h \
    custom_widgets/customshadoweffect.h \
    custom_widgets/iconbutton.h \
    custom_widgets/qcustomlabel.h \
    custom_widgets/taskprogressbar.h \
    helpers/iconmanager.h \
    models/date.h \
    models/task.h \
    settings/constants.h \
    views/mainwindow.h \
    views/taskdialog.h

FORMS    += \
    views/mainwindow.ui \
    views/taskdialog.ui

RESOURCES += \
    resources.qrc
