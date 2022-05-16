#include "appcontroller.h"
#include "saleitem.h"
#include "mainwindow.h"
#include "purchases.h"
#include "decompresser.h"
#include "appsettings.h"

#include <QDebug>
#include <QStringList>
#include <QFileDialog>
#include <QTreeView>
#include <QDate>
#include <qmessagebox.h>
#include <qsettings.h>
#include <QApplication>
#include <QStandardPaths>

#include <QStandardItemModel>
#include <exchangerates.h>


AppController::AppController(MainWindow* mainwindow) {
    m_mainWindow = mainwindow;

    AppSettings *appSettings = AppSettings::instance();
    int errorType;
    if (!appSettings->initialize(errorType)) {
        if (errorType == AppSettings::RATES_DIR_NOT_EXIST)
            QMessageBox::warning(mainwindow, "Warning", "Please browse to the directory containing the exchange rates.");
        if (errorType == AppSettings::NO_RATES_FOR_BASE_CURRENCY)
            QMessageBox::warning(mainwindow,
                                 tr("Warning"),
                                 tr("There are no exchange rates for the sellected base currency inside \"exchange_rates\" directory"));
        if (errorType == AppSettings::REPORTS_DIR_NOT_EXIST)
            qDebug() << "Please browse to the Daily Reports directory";
    }
}

AppController::~AppController() {
//    delete request;
}

QList<QDate> AppController::loadAppleReportFiles(QString path, MainWindow *mainwindow) {
    QStringList nameFilter;
    nameFilter << "*.txt.gz";
    QFileInfoList fileInfoList = QDir(path).entryInfoList(nameFilter, QDir::Dirs | QDir::Files
                                                          | QDir::NoSymLinks | QDir::NoDot
                                                          | QDir::NoDotDot);
    QStringList pathList;
    for (auto &fileInfo : fileInfoList) {
        pathList << fileInfo.absoluteFilePath();
    }

    if (pathList.isEmpty()) {
        QMessageBox::warning( mainwindow, "Warning", "There are no Daily Report Files in the selected folder." );
        return QList<QDate>{};
    }
    QList <QDate> dateOfReportList = getDatesOfReports(pathList);

    mDirectory = path;

    QString message = getMissingDates(dateOfReportList);
    if (message.length() > 0) {
        QMessageBox::warning( mainwindow, "Warning", message );
    }

    return dateOfReportList;
}

QString AppController::dailyReportsDirPath() {
    return m_dailyReportsDirPath;
}

QList <QDate> AppController::getDatesOfReports(QStringList list) {
    QList <QDate> dateOfReportList;
    for (int i = 0; i < list.size(); ++i) {
        std::string t = list.at(i).toLocal8Bit().constData();
        QString s = QString::fromStdString(t);
        s = s.right(15);
        s = s.left(8);
        dateOfReportList << QDate::fromString(s, "yyyyMMdd");
    }
    mAllDatesList = dateOfReportList;
    return dateOfReportList;
}

QString AppController::getMissingDates(QList <QDate> list) {
    QString message = "";
    QDate firstDate = list.first();
    QDate lastDate = list.last();

    QList <QDate> missingDatesList;

    int dateListIndex = 1;
    while(firstDate < lastDate) {
        firstDate = firstDate.addDays(1);
        if(firstDate != list[dateListIndex])
            missingDatesList << firstDate;
        else
            dateListIndex++;
    }

    if (missingDatesList.count() > 0) {
        message = "There are missing reports for the following dates: \n\n";
        for (int i = 0; i < missingDatesList.count(); i++) {
            message.append(missingDatesList[i].toString() + "\n");
        }
    }
    return message;
}


QStandardItemModel *AppController::purchasesModel(const QDate &fromDate, const QDate &toDate) {
    //TODO: resolve memory leak

    Purchases *purchases = new Purchases(fromDate, toDate);
//    qDebug() << purchases->purchasesModel()->rowCount();
    return purchases->purchasesModel();
}

bool AppController::isDailyReport(QDate date) {
    for (int i = 0; i < mAllDatesList.count(); i++) {
        if (date == (mAllDatesList)[i])
            return true;
    }
    return false;
}

