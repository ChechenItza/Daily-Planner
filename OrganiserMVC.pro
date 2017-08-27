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


SOURCES += controllers/datecontrollers.cpp \
    controllers/db.cpp \
    custom_widgets/qcustomlabel.cpp \
    custom_widgets/taskprogressbar.cpp \
    custom_widgets/myiconbutton.cpp \
    custom_widgets/mycustomshadoweffect.cpp \
    custom_widgets/materialdialog.cpp \
    helpers/iconmanager.cpp \
    models/date.cpp \
    views/mainwindow.cpp \
    views/taskdialog.cpp \
    main.cpp \
    custom_widgets/daytaskwidget.cpp \
    custom_widgets/daywidget.cpp \
    custom_widgets/emptydaywidget.cpp \
    controllers/groupcontainer.cpp \
    controllers/taskcontainer.cpp \
    models/task.cpp

HEADERS  += controllers/datecontrollers.h \
    controllers/db.h \
    custom_widgets/qcustomlabel.h \
    custom_widgets/taskprogressbar.h \
    custom_widgets/myiconbutton.h \
    custom_widgets/mycustomshadoweffect.h \
    custom_widgets/materialdialog.h \
    helpers/iconmanager.h \
    models/date.h \
    settings/constants.h \
    views/mainwindow.h \
    views/taskdialog.h \
    custom_widgets/daytaskwidget.h \
    custom_widgets/daywidget.h \
    custom_widgets/emptydaywidget.h \
    models/group.h \
    models/break.h \
    controllers/groupcontainer.h \
    controllers/taskcontainer.h \
    models/tasktemplate.h \
    models/task.h

FORMS    += views/mainwindow.ui \
    views/taskdialog.ui \
    custom_widgets/materialdialog.ui \

RESOURCES += \
    resources.qrc
