#include "appcontroller.h"
#include "httprequest.h"
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
    request = new HttpRequest(mainwindow, "http://www.wingssystems.com/index.php/rest_server/all_authors_designs/format/json", "");
    connect(request, SIGNAL(requestIsFinished()), this, SLOT(requestIsFinished()));
    artisticRequest = new HttpRequest(mainwindow, "http://www.wingssystems.com/Artistic_Snap/index.php/rest_server/all_authors_designs/format/json", "");
    connect(artisticRequest, SIGNAL(requestIsFinished()), this, SLOT(artisticRequestIsFinished()));

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
    getAuthorList();
}

AppController::~AppController() {
    delete request;
}

void AppController::requestIsFinished() {
    emit authorsDesignRespReceived(request->authorSingularList);
}

void AppController::artisticRequestIsFinished() {
    foreach( QString key, artisticRequest->designsAuthors.keys() ) {
        request->designsAuthors.insert(key, artisticRequest->designsAuthors.value(key));
        //qDebug() << key << artisticRequest->designsAuthors.value(key);
    }
    foreach( QString key, artisticRequest->designsWingsPercentageMap.keys() ) {
        request->designsWingsPercentageMap.insert(key, artisticRequest->designsWingsPercentageMap.value(key));
        //qDebug() << key << artisticRequest->designsWingsPercentageMap.value(key);
    }
    //        for (int i = 0; i < artisticRequest->authorSingularList->count(); i++)
    //        {
    //            request->authorSingularList.insert(key, artisticRequest->authorSingularList->value(key));
    //            qDebug() << key << artisticRequest->authorSingularList->value(key);
    //        }

    emit artisticAuthorsDesignRespReceived(artisticRequest->authorSingularList);
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

QString AppController::getMissingDates(QList <QDate> list)
{
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

    ////      if (authorValue == "DRAWings Snap"){authorList << "Wings Systems";}
    ////      else {
    ////        authorList << request->designsAuthors.value(authorValue);
    ////      }
    ////      QString developerProceedsString = list[8].replace("	", "");
    ////      if (developerProceedsString.left(1) == ".")
    ////      {
    ////        developerProceedsString = "0" +developerProceedsString;
    ////      }
    ////      developerProceedsList << developerProceedsString.toFloat();

    ////      QString customerPriceString = list[15].replace("	", "");
    ////      if (customerPriceString.left(1) == ".")
    ////      {
    ////        customerPriceString = "0" +customerPriceString;
    ////      }
    ////      customerPriceList << customerPriceString.toFloat();
}

//void AppController::populateSaleItemsPerAuthorMap(QMap <QString, QList <SaleItem*>* > *saleItemsPerAuthorMap, MainWindow* mainwindow,
//                                                  QDate sinceDate, QDate untilDate,

//    // Creates one AuthorList object for each Author and inserts them into a map
//    for (int i = 0; i < authorsSelectedList.count(); i++) {
//        QList <SaleItem*>* saleItemList = new QList <SaleItem*>;
//        saleItemsPerAuthorMap->insert(authorsSelectedList[i], saleItemList);
//    }

//    while(sinceDate <= untilDate) {
//        if(isDailyReport(sinceDate))
//            populateAllSaleItemList(saleItemsPerAuthorMap, &sinceDate, mainwindow);
//        sinceDate = sinceDate.addDays(1);
//    }
//    return;
//}


bool AppController::isDailyReport(QDate date) {
    for (int i = 0; i < mAllDatesList.count(); i++) {
        if (date == (mAllDatesList)[i])
            return true;
    }
    return false;
}

QStringList* AppController::getAuthorList() {
    qDebug() << request->authorSingularList;
    return request->authorSingularList;
}

void AppController::populateAllSaleItemList(QMap<QString, QList<SaleItem *>* >* saleItemsPerAuthorMap, QDate *date, MainWindow *mainwindow)
{
//    QString destinationPath;// = unpackDailyReportFile(*date);
//    QFile destinationFile(destinationPath);

//    destinationFile.open(QIODevice::ReadOnly);
//    QStringList fileLineList;
//    QTextStream textStream(&destinationFile);
//    while (true)
//    {
//        QString line = textStream.readLine();
//        //        if (line.contains("	-"))
//        //        {
//        //            QApplication::restoreOverrideCursor();
//        //            QMessageBox::warning(mainwindow, "Warning", "There are negative revenues! Check the \"Revenue\" column.");
//        //            QApplication::setOverrideCursor(Qt::WaitCursor);
//        //        }
//        if (line.isNull())
//            break;
//        else
//            fileLineList.append(line);
//    }

//    for (int i = 1; i < fileLineList.size(); ++i) {
//        std::string t = fileLineList.at(i).toLocal8Bit().constData();
//        QString s = QString::fromStdString(t);
//        QRegExp rx("((\\w|[.]|[/]|[ ]|[-])+(\\t))");
//        QStringList list;
//        int pos = 0;

//        while ((pos = rx.indexIn(s, pos)) != -1) {
//            list << rx.cap(1);
//            pos += rx.matchedLength();
//        }

//        QString authorValue = QString::fromStdString(list[4].toStdString()).replace("	", "");
//        if (authorValue == "DRAWings Snap"){authorValue = "Wings Systems";}
//        else {authorValue = request->designsAuthors.value(authorValue);}

//        QString skuString = QString::fromStdString(list[2].toStdString()).replace("	", "");

//        if (skuString == "M00Edit_Module" || skuString == "J00Edit_Module" || skuString == "M01Text_Module" || skuString == "J01Text_Module")
//        {
//            authorValue = "Wings Systems";
//        }
//        qDebug() << *saleItemsPerAuthorMap;
//        //        if (!(*saleItemsPerAuthorMap).contains(authorValue))// && skuString != "M00Edit_Module" && skuString != "J00Edit_Module" && skuString != "M01Text_Module" && skuString != "J01Text_Module")
//        //        {
//        //            if(!request->authorSingularList->contains(authorValue))
//        //            {
//        //                QApplication::restoreOverrideCursor();
//        //                QMessageBox::warning(mainwindow, "Warning", "The following uknown authors has been found: \"" + authorValue + "\"");
//        //                QApplication::setOverrideCursor(Qt::WaitCursor);
//        //            }
//        //            continue;
//        //        }


//        //qDebug() << authorValue;
//        SaleItem* saleItem = new SaleItem;
//        (*saleItemsPerAuthorMap)[authorValue]->append(saleItem);

//        saleItem->provider = QString::fromStdString(list[0].toStdString()).replace("	", "");
//        saleItem->providerCountry = QString::fromStdString(list[1].toStdString()).replace("	", "");
//        //        QString skuString = QString::fromStdString(list[2].toStdString()).replace("	", "");
//        saleItem->sku = skuString;
//        saleItem->developer = QString::fromStdString(list[3].toStdString()).replace("	", "");
//        QString titleString = QString::fromStdString(list[4].toStdString()).replace("	", "");
//        saleItem->title = titleString;
//        saleItem->version = QString::fromStdString(list[5].toStdString()).replace("	", "");
//        QString productTypeIdentifier = QString::fromStdString(list[6].toStdString()).replace("	", "");
//        saleItem->productTypeIdentifier = productTypeIdentifier;
//        QString units = QString::fromStdString(list[7].toStdString()).replace("	", "");
//        saleItem->units = units;
//        QString developerProceedsString = list[8].replace("	", "");
//        if (developerProceedsString.left(1) == ".") {
//            developerProceedsString = "0" +developerProceedsString;
//        } else if (developerProceedsString.left(2) == "-.") {
//            developerProceedsString = developerProceedsString.left(developerProceedsString.length()-2);
//            developerProceedsString = "-0." +developerProceedsString;
//        }

//        int sign = 1;
//        if (units.toInt() < 0) sign = -1;
//        float developerProceeds = sign * developerProceedsString.toFloat()/0.7;
//        // if (developerProceeds <0) qDebug() << "saleItem->developerProceeds" << developerProceeds;

//        saleItem->developerProceeds = developerProceeds;
//        QString beginDate = QString::fromStdString(list[9].toStdString()).replace("	", "");
//        saleItem->beginDate = beginDate;
//        saleItem->endDate = QString::fromStdString(list[10].toStdString()).replace("	", "");
//        saleItem->customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
//        saleItem->countryCode = QString::fromStdString(list[12].toStdString()).replace("	", "");
//        saleItem->currencyOfProceeds = QString::fromStdString(list[13].toStdString()).replace("	", "");
//        saleItem->appleIdentifier = QString::fromStdString(list[14].toStdString()).replace("	", "");
//        saleItem->customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
//        QString customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
//        saleItem->customerCurrency = customerCurrency;
//        QString customerPriceString = list[15].replace("	", "");
//        QString parentIdentifierString = list[17].replace("	", "");



//        if (productTypeIdentifier == "7" || productTypeIdentifier == "7F" || productTypeIdentifier == "7T" || productTypeIdentifier == "F7") {
//            realNameString = realNameString.append(" - update");
//        }


//        saleItem->realName = realNameString;

//        if (customerPriceString.left(1) == ".") {
//            customerPriceString = ("0" +customerPriceString);
//        }
//        else if (developerProceedsString.left(2) == "-.") {
//            customerPriceString = customerPriceString.left(customerPriceString.length()-2);
//            customerPriceString = "-0." +customerPriceString;
//        }

//        float customerPrice = customerPriceString.toFloat();
//        //if (customerPrice < 0) qDebug() <<  "saleIteme->customerPrice: " <<customerPrice;

//        saleItem->customerPrice = customerPrice;
//        saleItem->author = authorValue;
//        saleItem->wingsPercentage = request->designsWingsPercentageMap.value(titleString);
//        //qDebug() << designName << request->designsWingsPercentageMap.value(designName);
//        saleItem->date = *date;

//        ExchangeRates::RateErrorType error;
//        float rate = ExchangeRates::instance()->rate(customerCurrency,
//                                                     date->toString("yyyy-MM-dd"),
//                                                     error);
//        switch (error) {
//        case ExchangeRates::NO_DATA_FOR_CURRENCY:
//            qDebug() << "Error! no rates for " << customerCurrency << "!";
//            break;
//        case ExchangeRates::NO_DATA_FOR_DATE:
//            qDebug() << "Error! No rate for " << customerCurrency
//                     << " on " << date->toString("yyyy-MM-dd") << "!";
//            break;
//        default:
//            break;
//        }
//        saleItem->developerProceedsinEuros = developerProceeds*rate;

//        //        //Check if the Revenue is negative and append to Comments
//        //        if (customerPrice < 0)
//        //        {
//        //            mCommentString.append(authorValue + "'s " +  realNameString + " on " + beginDate + ", ");
//        //        }
//    }

//    destinationFile.close();

//    // Remove temporary file after DailyReport object is created.
//    destinationFile.remove();
}

