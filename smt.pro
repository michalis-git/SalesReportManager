#-------------------------------------------------
#
# Project created by QtCreator 2013-09-18T15:46:16
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += script
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = unpack
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dailyreport.cpp \
    appcontroller.cpp \
    httprequest.cpp \
    currency.cpp \
    saleitem.cpp \
    reports/subsidiaryledgerrep.cpp \
    reports/bycountryrep.cpp \
    reports/balancesheetrep.cpp



HEADERS  += mainwindow.h \
    dailyreport.h \
    appcontroller.h \
    currency.h \
    saleitem.h \
    httprequest.h \
    reports/subsidiaryledgerrep.h \
    reports/bycountryrep.h \
    reports/balancesheetrep.h


FORMS    += mainwindow.ui

INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
LIBS += -L/usr/local/opt/zlib/lib -lz
#INCLUDEPATH += C:\Qt\Qt5.0.2\5.0.2\mingw47_32\include\QtZlib
#INCLUDEPATH += C:\Qt\Qt5.1.1VS\5.1.1\msvc2012\include\QtZlib
#INCLUDEPATH += C:\Qt\Qt5.1.1VS\5.1.1\msvc2012\lib
#LIBS += -lz

RESOURCES += \
    resources.qrc