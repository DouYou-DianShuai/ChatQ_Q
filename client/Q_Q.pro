#-------------------------------------------------
#
# Project created by QtCreator 2021-07-05T14:41:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Q_Q
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        widget.cpp \
    register.cpp \
    q_qchat.cpp \
    addfriend.cpp \
    addgroup.cpp \
    creategroup.cpp \
    chatwidget.cpp \
    groupchatwidget.cpp \
    sendfilethread.cpp \
    recvfilethread.cpp

HEADERS += \
        widget.h \
    register.h \
    q_qchat.h \
    addfriend.h \
    addgroup.h \
    creategroup.h \
    chatwidget.h \
    groupchatwidget.h \
    sendfilethread.h \
    recvfilethread.h

FORMS += \
        widget.ui \
    register.ui \
    q_qchat.ui \
    addfriend.ui \
    addgroup.ui \
    creategroup.ui \
    chatwidget.ui \
    groupchatwidget.ui

RESOURCES += \
    aaa.qrc

DISTFILES +=
