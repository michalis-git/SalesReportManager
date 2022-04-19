#include "appcontroller.h"
#include "httprequest.h"
#include "saleitem.h"
#include "mainwindow.h"

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

#define EXCHANGE_RATES_DIR_PATH "/exchange_rates";
#define DAILY_REPORTS_DIR_PATH "/apple_daily_reports";


AppController::AppController(MainWindow* mainwindow) {
    request = new HttpRequest(mainwindow, "http://www.wingssystems.com/index.php/rest_server/all_authors_designs/format/json", "");
    connect(request, SIGNAL(requestIsFinished()), this, SLOT(requestIsFinished()));
    artisticRequest = new HttpRequest(mainwindow, "http://www.wingssystems.com/Artistic_Snap/index.php/rest_server/all_authors_designs/format/json", "");
    connect(artisticRequest, SIGNAL(requestIsFinished()), this, SLOT(artisticRequestIsFinished()));

    m_ratesDirPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            + EXCHANGE_RATES_DIR_PATH;

    if (!QDir(m_ratesDirPath).exists()) {
        QMessageBox::warning(mainwindow, "Warning", "Please browse to the directory containing the exchange rates.");
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

    getRates(dateOfReportList, mainwindow);
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

int AppController::onDateClicked(QDate date) {
    createDayReport(date);
    int numberOfRows = mDailyReport->providerList.count();
    return numberOfRows;
}

void AppController::createDayReport(QDate date) {
    QString destinationPath = unpackDailyReportFile(date);
    QFile destinationFile(destinationPath);

    //Parse the selected DailyReport file and create the DailyReport object.
    parseDailyReport(&destinationFile, date);

    // Remove temporary file after DailyReport object is created.
    destinationFile.remove();
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
    if (!QDir(folderPath).exists())
    {
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

void AppController::parseDailyReport(QFile *file, QDate date) {
    mDailyReport = new DailyReport;

    file->open(QIODevice::ReadOnly);
    QStringList fileLineList;
    QTextStream textStream(file);
    while (true)
    {
        QString line = textStream.readLine();
        if (line.contains("	-")) { qDebug() << "negative revenue"; }
        if (line.isNull())
            break;
        else
            fileLineList.append(line);
    }

    QStringList providerList, providerCountryList, skuList, authorList, developerList, titleList,
            versionList, productTypeIdentifierList, beginDateList, endDateList, customerCurrencyList,
            countryCodeList, currencyOfProceedsList, parentIdentifierList, realNameList;
    QVector<int> unitsList;
    QVector<float> developerProceedsList;
    QVector<int> appleIdentifierList;
    QVector<float>customerPriceList;

    for (int i = 1; i < fileLineList.size(); ++i)
    {
        std::string t = fileLineList.at(i).toLocal8Bit().constData();
        QString s = QString::fromStdString(t);
        QRegExp rx("((\\w|[.]|[/]|[ ]|[-])+(\\t))");
        QStringList list;
        int pos = 0;

        while ((pos = rx.indexIn(s, pos)) != -1) {
            list << rx.cap(1);
            pos += rx.matchedLength();
        }

        providerList << QString::fromStdString(list[0].toStdString()).replace("	", "");
        providerCountryList << QString::fromStdString(list[1].toStdString()).replace("	", "");
        QString skuString = QString::fromStdString(list[2].toStdString()).replace("	", "");
        skuList << skuString;
        QString titleString = QString::fromStdString(list[4].toStdString()).replace("	", "");
        QString authorValue = titleString;
        if (authorValue == "DRAWings Snap"){authorList << "Wings Systems";}
        else {
            authorList << request->designsAuthors.value(authorValue);
        }
        developerList << QString::fromStdString(list[3].toStdString()).replace("	", "");
        titleList << QString::fromStdString(list[4].toStdString()).replace("	", "");
        versionList << QString::fromStdString(list[5].toStdString()).replace("	", "");
        productTypeIdentifierList << QString::fromStdString(list[6].toStdString()).replace("	", "");
        unitsList << list[7].toInt();

        QString developerProceedsString = list[8].replace("	", "");
        if (developerProceedsString.left(1) == ".")
        {
            developerProceedsString = "0" +developerProceedsString;
        }
        developerProceedsList << developerProceedsString.toFloat();

        QString beginDateString = QString::fromStdString(list[9].toStdString());
        beginDateString.replace("	", "");
        beginDateList << beginDateString;
        QString endDateString = QString::fromStdString(list[10].toStdString());
        endDateString.replace("	", "");
        endDateList << endDateString;

        customerCurrencyList << QString::fromStdString(list[11].toStdString()).replace("	", "");
        countryCodeList << QString::fromStdString(list[12].toStdString()).replace("	", "");
        QString currencyOfProceedsString =  QString::fromStdString(list[13].toStdString()).replace("	", "");
        currencyOfProceedsList << currencyOfProceedsString;
        appleIdentifierList << list[14].toInt();

        QString customerPriceString = list[15].replace("	", "");
        if (customerPriceString.left(1) == ".")
        {
            customerPriceString = "0" +customerPriceString;
        }
        customerPriceList << customerPriceString.toFloat();
        if (list.count() > 17) {
            QString parentIdentifier = QString::fromStdString(list[17].toStdString()).replace("	", "");
            parentIdentifierList << parentIdentifier;
        }


        //        QString realNameString = titleString;
        //        if (parentIdentifierString.isEmpty())
        //        {
        //            if (skuString == "DRAWings.Snap.X001")
        //            {
        //                realNameList << "MacDRAWings";
        //                //parentIdentifierString = "-";
        //            }
        //            else
        //            {
        //                realNameList << "Drawings Snap";
        //                //parentIdentifierString = "-";
        //            }
        //        }
        //        else
        //        {
        //            if (titleString == "00Edit_Module" | titleString == "M00Edit_Module")
        //            {
        //                realNameList << "Editing Module";
        //            }
        //            else if (titleString == "DRAWings Snap")
        //            {
        //                realNameList << "sss";
        //            }
        //            else
        //            {
        //                QString formatedName = titleString.left(titleString.length() - 8);
        //                if (formatedName.right(1) == ("_"))
        //                {
        //                    formatedName = formatedName.left(formatedName.length() - 1);
        //                    formatedName.append(" (Pack.) ");
        //                }
        //                realNameList << formatedName;
        //            }
        //        }
        //        if (parentIdentifierList.right(4) == "X001")
        //        {
        //            parentIdentifierList = "MacDRAWings";
        //        }
        //        else if (parentIdentifierList.right(4) == ".001")
        //        {
        //            parentIdentifierList = "Drawings Snap";
        //        }
        //        else
        //        {
        //            parentIdentifierList = "-";
        //        }
    }
    file->close();

    mDailyReport->setDate(&date);
    mDailyReport->providerList = providerList;
    mDailyReport->providerCountryList = providerCountryList;
    mDailyReport->skuList = skuList;
    mDailyReport->authorNameList = authorList;
    mDailyReport->developerList = developerList;
    mDailyReport->titleList = titleList;
    mDailyReport->versionList = versionList;
    mDailyReport->productTypeIdentifierList = productTypeIdentifierList;
    mDailyReport->unitsList = unitsList;
    mDailyReport->developerProceedsList = developerProceedsList;
    mDailyReport->beginDateList = beginDateList;
    mDailyReport->endDateList = endDateList;
    mDailyReport->customerCurrencyList = customerCurrencyList;
    mDailyReport->countryCodeList = countryCodeList;
    mDailyReport->currencyOfProceedsList = currencyOfProceedsList;
    mDailyReport->appleIdentifierList = appleIdentifierList;
    mDailyReport->customerPriceList = customerPriceList;
    mDailyReport->parentIdentifierList = parentIdentifierList;
    mDailyReport->realNameList = realNameList;
    // maDailyReport is deleted in mainwindow, after its data are loaded on a table.
}

void AppController::getRates(QList<QDate> dateOfReportList,  MainWindow *mainwindow) {
    QStringList fullPathList;
    fullPathList = createFullPathList();
    if (fullPathList.isEmpty()) {return;}

    //Gets the lastDateOfRates from a file in Rates Folder
    QDate lastDateOfRates  = getLastDateFromRateFile(fullPathList.first(), mainwindow);

    if (dateOfReportList.last() > lastDateOfRates.addDays(3)) {
        QString message =  "It has been more than 3 days since the last update of the exchange rates.";
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(mainwindow, "Warning", message);
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    fullPathList = createFullPathList();
    loadAllRates(fullPathList);
}

QStringList AppController::createFullPathList()
{
    QStringList fullPathList;
    QStringList pathList;
    QDir rateDirectory(m_ratesDirPath);
    QStringList nameFilter;
    nameFilter << "*.txt";
    pathList = rateDirectory.entryList(nameFilter, QDir::Files);

    //Creates full path list for the rates files.
    for (int i = 0; i < pathList.size(); ++i)
    {
        std::string t = pathList.at(i).toLocal8Bit().constData();
        QString s = QString::fromStdString(t);
        fullPathList << m_ratesDirPath + "/" + s;
    }

    //Creates Rate Files
    if (fullPathList.isEmpty())
    {
        return fullPathList;
    }
    return fullPathList;
}

QDate AppController::getLastDateFromRateFile(QString filePath, MainWindow* mainwindow)
{
    QFile exchangeRateFile(filePath);
    exchangeRateFile.open(QIODevice::ReadOnly);
    if(!exchangeRateFile.isOpen()){
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(mainwindow, "Warning", "Unable to open exchange rate file: " + filePath);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        mainwindow->close();
    }

    QStringList fileLineList;
    QTextStream textStream(&exchangeRateFile);
    while (true)
    {
        QString line = textStream.readLine();
        if (line.isNull())
            break;
        else
            fileLineList.append(line);
    }

    QStringList datesFromRateFileList;
    for (int i = 0; i < fileLineList.size(); ++i)
    {
        std::string t = fileLineList.at(i).toLocal8Bit().constData();
        QString s = QString::fromStdString(t);
        QRegExp rx("((\\w|[.]|[-])+(\\s))");

        int pos = 0;

        while ((pos = rx.indexIn(s, pos)) != -1) {
            datesFromRateFileList << rx.cap(0).replace(" ", "");
            pos += rx.matchedLength();
        }
    }
    QDate firstDate;
    firstDate = QDate::fromString(QString(datesFromRateFileList.last()), "yyyy-MM-dd");
    return firstDate;
}

void AppController::loadAllRates(QStringList fullPathList) {
    QStringList nameFilter;
    nameFilter << "*.txt.gz";
    QFileInfoList fileInfoList = QDir(m_ratesDirPath).entryInfoList(nameFilter, QDir::Dirs | QDir::Files
                                                                 | QDir::NoSymLinks | QDir::NoDot
                                                                 | QDir::NoDotDot);
    QStringList pathList;
    for (auto &fileInfo : fileInfoList) {
        QMap <QString, float> ratesMap;
        QString allRatesKey = fileInfo.baseName().toUpper();
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            QStringList fileLineList;
            QTextStream textStream(&file);
            QString previousDate = "";
            float rateTotal = 0;
            int rateCounter = 0;
            while (true) {
                QString line = textStream.readLine();
                if (line.isNull())
                    break;
                else {
                    fileLineList.append(line);
                    if (line.left(10) == previousDate) {
                        rateTotal += line.right(line.length() - 11).toFloat();
                        rateCounter++;
                        previousDate = line.left(10);
                    } else {
                        if(previousDate != "")
                            ratesMap.insert(previousDate, rateTotal/rateCounter);
                        ratesMap.insert(line.left(10), line.right(line.length() - 11).toFloat());
                        previousDate = line.left(10);
                        rateCounter = 1;
                        rateTotal = line.right(line.length() - 11).toFloat();
                    }
                    ratesMap.insert(line.left(10), line.right(line.length() - 11).toFloat());
                }
            }
        }


        mAllRatesMap.insert(allRatesKey, ratesMap);
    }
}

QStringList* AppController::getAuthorList()
{
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

    for (int i = 1; i < fileLineList.size(); ++i)
    {
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
            if (skuString == "DRAWings.Snap.X001")
            {
                realNameString = "MacDRAWings";
            }
            else if (skuString == "Drawings.Snap.001")
            {
                realNameString = "Drawings Snap";
            }
            else if (skuString == "Snap.001")
            {
                realNameString = "Artistic Snap";
            }
            else
            {
                realNameString = "UKNOWN APPLICATION!";
                QApplication::restoreOverrideCursor();
                QMessageBox::warning(mainwindow, "Warning", "There is at least one Uknown Application in the report(s).");
                QApplication::setOverrideCursor(Qt::WaitCursor);
            }
        }
        else
        {
            if (skuString == "00Edit_Module" || skuString == "M00Edit_Module" || skuString == "J00Edit_Module")
            {
                realNameString = "Edit. Module";
            }
            else if (skuString == "01Text_Module" || skuString == "M01Text_Module" || skuString == "J01Text_Module")
            {
                realNameString = "Text Module";
            }
            else
            {
                QString formatedName = titleString.left(titleString.length() - 8);
                if (formatedName.right(1) == ("_"))
                {
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

        if (productTypeIdentifier == "7" || productTypeIdentifier == "7F" || productTypeIdentifier == "7T" || productTypeIdentifier == "F7")
        {
            realNameString = realNameString.append(" - update");
        }

        if (parentIdentifierString == "DRAWings.Snap.X001")
        {
            realNameString.append(" (Mac)");
            saleItem->parentIdentifier = "MacDRAWings";
        }
        else if (parentIdentifierString == "Drawings.Snap.001")
        {
            realNameString.append(" (Draw.)");
            saleItem->parentIdentifier = "Drawings Snap";
        }
        else if (parentIdentifierString == "Snap.001")
        {
            realNameString.append(" (Art.)");
            saleItem->parentIdentifier = "Artistic Snap";
        }
        else if (parentIdentifierString == " ")
        {
            saleItem->parentIdentifier = "-";
        }
        else
        {
            saleItem->parentIdentifier = "UKNOWN PARENT!";
            QApplication::restoreOverrideCursor();
            QMessageBox::warning(mainwindow, "Warning", "There is at least one Uknown Download Source (see \"Downloaded from\" column) in the report(s).");
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }

        saleItem->realName = realNameString;

        if (customerPriceString.left(1) == ".")
        {
            customerPriceString = ("0" +customerPriceString);
        }
        else if (developerProceedsString.left(2) == "-.")
        {
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
        float firstRate  = (mAllRatesMap[customerCurrency]).begin().value();
        float rate = 0;
        if ((mAllRatesMap[customerCurrency]).contains((*date).toString("yyyy-MM-dd")))
        {
            rate  = (mAllRatesMap[customerCurrency])[(*date).toString("yyyy-MM-dd")];
            firstRate = rate;
        }
        else
        {
            rate = firstRate;
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

