#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T12:27:11
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = boids
TEMPLATE = app


SOURCES += main.cpp\
        princ.cpp \
        glarea.cpp \
    poisson.cpp \
    banc.cpp \
    obstacle.cpp

HEADERS  += princ.h \
        glarea.h \
    poisson.h \
    banc.h \
    obstacle.h

FORMS    += princ.ui

RESOURCES += \
    boids.qrc
