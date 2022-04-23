#include "appcontroller.h"
#include "httprequest.h"
#include "saleitem.h"
#include "mainwindow.h"
#include "purchases.h"

#include <QDebug>
#include <QStringList>
#include <QFileDialog>
#include <QTreeView>
#include <QDate>
#include "zlib.h"
//#include "C:/Qt/Qt5.1.1VS/5.1.1/msvc2012/include/QtZlib/zlib.h"
//#include "zlib.h"
#include <qmessagebox.h>
#include <qsettings.h>
#include <QTextStream>
#include <QApplication>
#include <QStandardPaths>

#include <QStandardItemModel>
#include <exchangerates.h>

#define EXCHANGE_RATES_DIR_PATH "/exchange_rates";
#define DAILY_REPORTS_DIR_PATH "/apple_daily_reports";


AppController::AppController(MainWindow* mainwindow) {
    m_mainWindow = mainwindow;
  request = new HttpRequest(mainwindow, "http://www.wingssystems.com/index.php/rest_server/all_authors_designs/format/json", "");
  connect(request, SIGNAL(requestIsFinished()), this, SLOT(requestIsFinished()));
  artisticRequest = new HttpRequest(mainwindow, "http://www.wingssystems.com/Artistic_Snap/index.php/rest_server/all_authors_designs/format/json", "");
  connect(artisticRequest, SIGNAL(requestIsFinished()), this, SLOT(artisticRequestIsFinished()));

  m_ratesDirPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                   + EXCHANGE_RATES_DIR_PATH;

  if (!QDir(m_ratesDirPath).exists()) {
    QMessageBox::warning(mainwindow, "Warning", "Please browse to the directory containing the exchange rates.");
  } else {
    ExchangeRates *rates = ExchangeRates::instance();
    if (!rates->initialize(m_ratesDirPath, "toEUR"))
      QMessageBox::warning(mainwindow,
                           tr("Warning"),
                           tr("There are no exchange rates for the sellected base currency inside \"exchange_rates\" directory"));
  }

  QSettings settings("SMT");
  QString defDailyReportsPath = QDir::homePath() + DAILY_REPORTS_DIR_PATH;
  m_dailyReportsDirPath = settings.value("dailyReportsDirPath", defDailyReportsPath).toString();
  if (!QDir(m_dailyReportsDirPath).exists())
    qDebug() << "Please browse to the Daily Reports directory";

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

QString AppController::getBrowsedPath(MainWindow* mainwindow) {
  QString path;

  QString defaultDirectoryPath = DAILY_REPORTS_DIR_PATH;
  if (QDir(defaultDirectoryPath).exists())
    defaultDirectoryPath = defaultDirectoryPath;
  else
    defaultDirectoryPath = QDir::homePath();

  QFileDialog fileDialog(mainwindow, "Open Directory", defaultDirectoryPath);

  fileDialog.setFileMode(QFileDialog::Directory);
  fileDialog.setOption(QFileDialog::ShowDirsOnly);
  fileDialog.setWindowModality(Qt::ApplicationModal);
  int result = fileDialog.exec();
  if (result == 1) {
    path = fileDialog.selectedFiles().first();
    return path;
  } else {
    path = "cancel";
    return path;
  }
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

QStandardItemModel *AppController::dayReportModel(const QDate &date) {
    //TODO: resolve memory leak
    QStandardItemModel *model = new QStandardItemModel;
    QString decompressedPath = unpackDailyReportFile(date);
    Purchases *purchases = new Purchases(date, decompressedPath);
    qDebug() << purchases->purchasesModel()->rowCount();
    return purchases->purchasesModel();
//    QFile file(decompressedPath);

//    if (file.open(QIODevice::ReadOnly)) {
//    QStringList fileLineList;
//    QTextStream textStream(&file);
//    while (true) {
//      QString line = textStream.readLine();
//      if (line.contains("	-"))
//          qDebug() << "negative revenue";
//      if (line.isNull())
//        break;
//      else
//        fileLineList.append(line);
//    }


//    QStringList titles = fileLineList.first().split("\t");
//    qDebug() << titles;
//    for (int i = 0; i< titles.count(); i++) {
//        QStandardItem *headerItem = new QStandardItem;
//        headerItem->setText(titles[i]);
//        model->setHorizontalHeaderItem(i, headerItem);
//    }

//    for (int i = 1; i < fileLineList.size(); ++i) {
//      std::string t = fileLineList.at(i).toLocal8Bit().constData();
//      QString s = QString::fromStdString(t);
//      QRegExp rx("((\\w|[.]|[/]|[ ]|[-])+(\\t))");
//      QStringList list;
//      int pos = 0;

//      while ((pos = rx.indexIn(s, pos)) != -1) {
//        list << rx.cap(1);
//        pos += rx.matchedLength();
//      }

//      for (int j = 0; j < list.count(); j++) {
//          QStandardItem *item = new QStandardItem;
//          item->setText(list[j]);
//          model->setItem(i - 1, j, item);
//      }
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
//    }
//    file.close();

//    }
    return model;
}

void AppController::populateSaleItemsPerAuthorMap(QMap <QString, QList <SaleItem*>* > *saleItemsPerAuthorMap, MainWindow* mainwindow, QDate sinceDate, QDate untilDate, QStringList authorsSelectedList, QString *missingDatesMessage)
{
  //    QMap <QString, QList<SaleItem*> > saleItemsPerAuthorMap;
  //    QList <SaleItem*> allSaleItemList;

     // Checks if there is a folder with Daily Report Files.
  if (mAllDatesList.isEmpty()) {
    QMessageBox::warning(mainwindow, "Warning", "You should select a folder with Daily Report Files.");
    return;
  }

     // Checks if the selected time period is valid.
  if (sinceDate > untilDate) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(mainwindow, "Warning", "The date in the first calendar must be earlier than the date in the second one.");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    return;
  }

     // Checks if there are Daily Report Files for the selected time period.
  if(!isDailyReport(sinceDate) || !isDailyReport(untilDate)) {
    QMessageBox::warning(mainwindow, "Warning", "You should select only the yellow dates.");
    return;
  }

  QList <QDate> missingDatesList;
  QDate startDate = sinceDate;
  while(startDate < untilDate) {
    if(!isDailyReport(startDate)) {
      missingDatesList << startDate;
    }
    startDate = startDate.addDays(1);
  }

  QString message = "";
  for (int i = 0; i < missingDatesList.count(); i++) {
    message = "There are missing reports for the following dates:\n";
    for (int i = 0; i < missingDatesList.count(); i++) {
      message.append(missingDatesList[i].toString() + ", ");
    }
    message = message.left(message.length() - 2);
    message.append(".");
  }
  if (message != "") {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(mainwindow, "Warning", message);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    *missingDatesMessage = message;
  } else {
    *missingDatesMessage = "";
  }

     // Creates one AuthorList object for each Author and inserts them into a map
  for (int i = 0; i < authorsSelectedList.count(); i++) {
    QList <SaleItem*>* saleItemList = new QList <SaleItem*>;
    saleItemsPerAuthorMap->insert(authorsSelectedList[i], saleItemList);
  }

  while(sinceDate <= untilDate) {
    if(isDailyReport(sinceDate))
      populateAllSaleItemList(saleItemsPerAuthorMap, &sinceDate, mainwindow);
    sinceDate = sinceDate.addDays(1);
  }
  return;
}

QString AppController::unpackDailyReportFile(QDate date) {
  // Gets the file name from the selected date and creates the full path of the file.
  QString dateStr = date.toString("yyyyMMdd");
  QStringList nameFilter;
  QStringList list;
  nameFilter << "*" + dateStr+ ".txt.gz";
  QDir dir(mDirectory);

  list = dir.entryList(nameFilter, QDir::Files);
  QString fullFilePath = mDirectory + "/" + list.first();

     // Decompresses the file in temporary file.
  QFile sourceFile(fullFilePath);
  QByteArray sourceData;

  if(sourceFile.open(QIODevice::ReadOnly))
  {
    QDataStream in1(&sourceFile);
    while(!in1.atEnd())
    {
      char *c = new char[1];
      in1.readRawData(c,1);
      sourceData.push_back(*c);
      delete []c;
    }
    sourceFile.close();
  }

     //Create tmp folder
  QString folderPath = QDir::home().path() + "/tmpApplReports";
  if (!QDir(folderPath).exists()) {
    QDir().mkdir(folderPath);
  }
  QString destinationFilePath = folderPath + "/" + dateStr + ".txt";
  QFile destinationFile(destinationFilePath);
  QByteArray destinationData = gzipDecompress(sourceData);
  destinationFile.open(QIODevice::WriteOnly);
  destinationFile.write(destinationData);
  destinationFile.close();

  return destinationFilePath;
}

bool AppController::isDailyReport(QDate date) {
  for (int i = 0; i < mAllDatesList.count(); i++) {
    if (date == (mAllDatesList)[i])
      return true;
  }
  return false;
}


QByteArray AppController::gzipDecompress(QByteArray &compressData ) {
  //strip header
  compressData.remove(0, 10);

  const int buffer_size = 16384;
  quint8 buffer[buffer_size];

  z_stream cmpr_stream;
  cmpr_stream.next_in = (unsigned char *)compressData.data();
  cmpr_stream.avail_in = compressData.size();
  cmpr_stream.total_in = 0;

  cmpr_stream.next_out = buffer;
  cmpr_stream.avail_out = buffer_size;
  cmpr_stream.total_out = 0;

  cmpr_stream.zalloc = Z_NULL;
  cmpr_stream.zfree = Z_NULL;
  cmpr_stream.opaque = Z_NULL;

  int status = inflateInit2( &cmpr_stream, -8 );
  if (status != Z_OK) {
    qDebug() << "cmpr_stream error!";
  }

  QByteArray uncompressed;
  do {
    cmpr_stream.next_out = buffer;
    cmpr_stream.avail_out = buffer_size;

    status = inflate( &cmpr_stream, Z_NO_FLUSH );

    if (status == Z_OK || status == Z_STREAM_END) {
      QByteArray chunk = QByteArray::fromRawData((char *)buffer, buffer_size - cmpr_stream.avail_out);
      uncompressed.append( chunk );
    } else {
      inflateEnd(&cmpr_stream);
      break;
    }

    if (status == Z_STREAM_END) {
      inflateEnd(&cmpr_stream);
      break;
    }
  }
  while (cmpr_stream.avail_out == 0);

  return uncompressed;
}

QStringList* AppController::getAuthorList() {
  return request->authorSingularList;
}

void AppController::populateAllSaleItemList(QMap<QString, QList<SaleItem *>* >* saleItemsPerAuthorMap, QDate *date, MainWindow *mainwindow)
{
  QString destinationPath = unpackDailyReportFile(*date);
  QFile destinationFile(destinationPath);

  destinationFile.open(QIODevice::ReadOnly);
  QStringList fileLineList;
  QTextStream textStream(&destinationFile);
  while (true)
  {
    QString line = textStream.readLine();
    //        if (line.contains("	-"))
    //        {
    //            QApplication::restoreOverrideCursor();
    //            QMessageBox::warning(mainwindow, "Warning", "There are negative revenues! Check the \"Revenue\" column.");
    //            QApplication::setOverrideCursor(Qt::WaitCursor);
    //        }
    if (line.isNull())
      break;
    else
      fileLineList.append(line);
  }

  for (int i = 1; i < fileLineList.size(); ++i) {
    std::string t = fileLineList.at(i).toLocal8Bit().constData();
    QString s = QString::fromStdString(t);
    QRegExp rx("((\\w|[.]|[/]|[ ]|[-])+(\\t))");
    QStringList list;
    int pos = 0;

    while ((pos = rx.indexIn(s, pos)) != -1) {
      list << rx.cap(1);
      pos += rx.matchedLength();
    }

    QString authorValue = QString::fromStdString(list[4].toStdString()).replace("	", "");
    if (authorValue == "DRAWings Snap"){authorValue = "Wings Systems";}
    else {authorValue = request->designsAuthors.value(authorValue);}

    QString skuString = QString::fromStdString(list[2].toStdString()).replace("	", "");

    if (skuString == "M00Edit_Module" || skuString == "J00Edit_Module" || skuString == "M01Text_Module" || skuString == "J01Text_Module")
    {
      authorValue = "Wings Systems";
    }
    qDebug() << *saleItemsPerAuthorMap;
    //        if (!(*saleItemsPerAuthorMap).contains(authorValue))// && skuString != "M00Edit_Module" && skuString != "J00Edit_Module" && skuString != "M01Text_Module" && skuString != "J01Text_Module")
    //        {
    //            if(!request->authorSingularList->contains(authorValue))
    //            {
    //                QApplication::restoreOverrideCursor();
    //                QMessageBox::warning(mainwindow, "Warning", "The following uknown authors has been found: \"" + authorValue + "\"");
    //                QApplication::setOverrideCursor(Qt::WaitCursor);
    //            }
    //            continue;
    //        }


       //qDebug() << authorValue;
    SaleItem* saleItem = new SaleItem;
    (*saleItemsPerAuthorMap)[authorValue]->append(saleItem);

    saleItem->provider = QString::fromStdString(list[0].toStdString()).replace("	", "");
    saleItem->providerCountry = QString::fromStdString(list[1].toStdString()).replace("	", "");
    //        QString skuString = QString::fromStdString(list[2].toStdString()).replace("	", "");
    saleItem->sku = skuString;
    saleItem->developer = QString::fromStdString(list[3].toStdString()).replace("	", "");
    QString titleString = QString::fromStdString(list[4].toStdString()).replace("	", "");
    saleItem->title = titleString;
    saleItem->version = QString::fromStdString(list[5].toStdString()).replace("	", "");
    QString productTypeIdentifier = QString::fromStdString(list[6].toStdString()).replace("	", "");
    saleItem->productTypeIdentifier = productTypeIdentifier;
    QString units = QString::fromStdString(list[7].toStdString()).replace("	", "");
    saleItem->units = units;
    QString developerProceedsString = list[8].replace("	", "");
    if (developerProceedsString.left(1) == ".")
    {
      developerProceedsString = "0" +developerProceedsString;
    }
    else if (developerProceedsString.left(2) == "-.")
    {
      developerProceedsString = developerProceedsString.left(developerProceedsString.length()-2);
      developerProceedsString = "-0." +developerProceedsString;
    }

    int sign = 1;
    if (units.toInt() < 0) sign = -1;
    float developerProceeds = sign * developerProceedsString.toFloat()/0.7;
    // if (developerProceeds <0) qDebug() << "saleItem->developerProceeds" << developerProceeds;

    saleItem->developerProceeds = developerProceeds;
    QString beginDate = QString::fromStdString(list[9].toStdString()).replace("	", "");
    saleItem->beginDate = beginDate;
    saleItem->endDate = QString::fromStdString(list[10].toStdString()).replace("	", "");
    saleItem->customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
    saleItem->countryCode = QString::fromStdString(list[12].toStdString()).replace("	", "");
    saleItem->currencyOfProceeds = QString::fromStdString(list[13].toStdString()).replace("	", "");
    saleItem->appleIdentifier = QString::fromStdString(list[14].toStdString()).replace("	", "");
    saleItem->customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
    QString customerCurrency = QString::fromStdString(list[11].toStdString()).replace("	", "");
    saleItem->customerCurrency = customerCurrency;
    QString customerPriceString = list[15].replace("	", "");
    QString parentIdentifierString = list[17].replace("	", "");

    QString realNameString = titleString;
    if (parentIdentifierString == " ")
    {
      if (skuString == "DRAWings.Snap.X001") {
        realNameString = "MacDRAWings";
      } else if (skuString == "Drawings.Snap.001") {
        realNameString = "Drawings Snap";
      } else if (skuString == "Snap.001") {
        realNameString = "Artistic Snap";
      } else {
        realNameString = "UKNOWN APPLICATION!";
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(mainwindow, "Warning", "There is at least one Uknown Application in the report(s).");
        QApplication::setOverrideCursor(Qt::WaitCursor);
      }
    } else {
      if (skuString == "00Edit_Module" || skuString == "M00Edit_Module" || skuString == "J00Edit_Module") {
        realNameString = "Edit. Module";
      } else if (skuString == "01Text_Module" || skuString == "M01Text_Module" || skuString == "J01Text_Module") {
        realNameString = "Text Module";
      } else {
        QString formatedName = titleString.left(titleString.length() - 8);
        if (formatedName.right(1) == ("_")) {
          formatedName = formatedName.left(formatedName.length() - 1);
          formatedName.append(" - PACK");
        }
        realNameString = formatedName;
      }
    }

       //        if (productTypeIdentifier == "FI1")
       //        {
       //            realNameString = realNameString.append(" (Mac)");
       //        }
       //        else if (productTypeIdentifier == "IA1" ||productTypeIdentifier == "IA9" || productTypeIdentifier == "IAY" || productTypeIdentifier == "IAC")
       //        {
       //            realNameString = realNameString.append(" (iOS)");
       //        }

    if (productTypeIdentifier == "7" || productTypeIdentifier == "7F" || productTypeIdentifier == "7T" || productTypeIdentifier == "F7") {
      realNameString = realNameString.append(" - update");
    }

    if (parentIdentifierString == "DRAWings.Snap.X001") {
      realNameString.append(" (Mac)");
      saleItem->parentIdentifier = "MacDRAWings";
    } else if (parentIdentifierString == "Drawings.Snap.001") {
      realNameString.append(" (Draw.)");
      saleItem->parentIdentifier = "Drawings Snap";
    } else if (parentIdentifierString == "Snap.001") {
      realNameString.append(" (Art.)");
      saleItem->parentIdentifier = "Artistic Snap";
    } else if (parentIdentifierString == " ") {
      saleItem->parentIdentifier = "-";
    } else {
      saleItem->parentIdentifier = "UKNOWN PARENT!";
      QApplication::restoreOverrideCursor();
      QMessageBox::warning(mainwindow, "Warning", "There is at least one Uknown Download Source (see \"Downloaded from\" column) in the report(s).");
      QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    saleItem->realName = realNameString;

    if (customerPriceString.left(1) == ".") {
      customerPriceString = ("0" +customerPriceString);
    }
    else if (developerProceedsString.left(2) == "-.") {
      customerPriceString = customerPriceString.left(customerPriceString.length()-2);
      customerPriceString = "-0." +customerPriceString;
    }

    float customerPrice = customerPriceString.toFloat();
    //if (customerPrice < 0) qDebug() <<  "saleIteme->customerPrice: " <<customerPrice;

    saleItem->customerPrice = customerPrice;
    saleItem->author = authorValue;
    saleItem->wingsPercentage = request->designsWingsPercentageMap.value(titleString);
    //qDebug() << designName << request->designsWingsPercentageMap.value(designName);
    saleItem->date = *date;

    ExchangeRates::RateErrorType error;
    float rate = ExchangeRates::instance()->rate(customerCurrency,
                                                 date->toString("yyyy-MM-dd"),
                                                 error);
    switch (error) {
      case ExchangeRates::NO_DATA_FOR_CURRENCY:
        qDebug() << "Error! no rates for " << customerCurrency << "!";
        break;
      case ExchangeRates::NO_DATA_FOR_DATE:
        qDebug() << "Error! No rate for " << customerCurrency
                 << " on " << date->toString("yyyy-MM-dd") << "!";
        break;
      default:
        break;
    }
    saleItem->developerProceedsinEuros = developerProceeds*rate;

       //        //Check if the Revenue is negative and append to Comments
       //        if (customerPrice < 0)
       //        {
       //            mCommentString.append(authorValue + "'s " +  realNameString + " on " + beginDate + ", ");
       //        }
  }

  destinationFile.close();

     // Remove temporary file after DailyReport object is created.
  destinationFile.remove();
}

