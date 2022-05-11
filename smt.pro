#-------------------------------------------------
#
# Project created by QtCreator 2013-09-18T15:46:16
#
#-------------------------------------------------

PROJECT_PATH = $$PWD

QT       += core gui
QT       += network
QT       += script
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Smt
TEMPLATE = app


SOURCES += main.cpp\
  appsettings.cpp \
  balancesheet.cpp \
  decompresser.cpp \
  exchangerates.cpp \
  financialreport.cpp \
        mainwindow.cpp \
    dailyreport.cpp \
    appcontroller.cpp \
    httprequest.cpp \
    currency.cpp \
  preferencesdialog.cpp \
  productsbycountry.cpp \
  property.cpp \
  purchase.cpp \
  purchases.cpp \
    saleitem.cpp \
    reports/subsidiaryledgerrep.cpp \
    reports/bycountryrep.cpp \
    reports/balancesheetrep.cpp \
  subsidiaryledgerreport.cpp



HEADERS  += mainwindow.h \
    appsettings.h \
    balancesheet.h \
    dailyreport.h \
    appcontroller.h \
    currency.h \
    decompresser.h \
    exchangerates.h \
    financialreport.h \
    preferencesdialog.h \
    productsbycountry.h \
    property.h \
    purchase.h \
    purchases.h \
    saleitem.h \
    httprequest.h \
    reports/subsidiaryledgerrep.h \
    reports/bycountryrep.h \
    reports/balancesheetrep.h \
    subsidiaryledger \
    subsidiaryledgerreport.h


FORMS    += mainwindow.ui \
  preferencesdialog.ui

INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
LIBS += -L/usr/local/opt/zlib/lib -lz

RESOURCES += \
    resources.qrc
