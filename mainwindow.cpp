#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "appcontroller.h"
#include "dailyreport.h"
#include "saleitem.h"

#include "reports/balancesheetrep.h"
#include "reports/subsidiaryledgerrep.h"
#include "reports/bycountryrep.h"

//#include "C:/Qt/Qt5.1.1VS/5.1.1/msvc2012/include/QtZlib/zlib.h"
#include "zlib.h"
#include "QFile"
#include "QFileInfo"
#include "QString"
#include "QDebug"
#include "QByteArray"
#include "QFileDialog"

#include "QDate"
#include <QList>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextCharFormat>
#include <QSettings>
#include <QTextEdit>
#include <QScrollBar>
#include <QLayoutItem>

bool authorLessThan(const SaleItem* left, const SaleItem* right) {
    if (left->realName == right->realName)
        return left->currencyOfProceeds < right->currencyOfProceeds;
    else
        return left->realName < right->realName;
}

bool countryLessThan(const SaleItem* left, const SaleItem* right) {
    if (left->realName == right->realName)
        return left->countryCode < right->countryCode;
    else
        return left->realName < right->realName;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    mAppController = new AppController(this);

    this->showMaximized();

    connect(ui->browseSourceToolButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
    connect(ui->calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(onDateClicked(QDate)));
    connect(ui->doneButton, SIGNAL(clicked()), this, SLOT(onDoneClicked()));
    connect(mAppController, SIGNAL(authorsDesignRespReceived(QStringList*)), this, SLOT(onAuthorsDesignsRespReceived(QStringList*)));
    connect(ui->actionExport_Report_as_HTML, SIGNAL(triggered()), this, SLOT(onExportReportClicked()));
    connect(ui->actionActionExport, SIGNAL(triggered()), this, SLOT(onExportReportClicked()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onQuitClicked()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListWidgetItemClicked(QListWidgetItem*)));

// Initialize Calendars
    ui->fromDateEdit->setDisplayFormat("dd/MM/yyyy");
    ui->fromDateEdit->setCalendarPopup(true);
    fromCalendarWidget = new QCalendarWidget;
    ui->fromDateEdit->setCalendarWidget(fromCalendarWidget);

    ui->toDateEdit->setDisplayFormat("dd/MM/yyyy");
    ui->toDateEdit->setCalendarPopup(true);
    toCalendarWidget = new QCalendarWidget;
    ui->toDateEdit->setCalendarWidget(toCalendarWidget);

    initializeTableView();
}

MainWindow::~MainWindow() {
    delete ui;
    delete mAppController;
    delete m_dailyModel;
    delete fromCalendarWidget;
    delete toCalendarWidget;
}

void MainWindow::initializeTableView() {
 //TODO: populate titleList from file
    // Initialize Tableview
        QStringList titleList;
        titleList << "Provider" << "Provider Country" << "SKU" << "Developer" << "Title" << "Version" << "Type Identifier"
                  << "Units" << "Developer Proceeds" << "Begin Date" << "End Date" << "Customer Currency "<< "Country Code"
                  << "Currency of Proceeds" << "Apple Identifier" << "Customer Prise" << "Author";
        m_dailyModel = new QStandardItemModel(10,titleList.count(),this);
        for (int i = 0; i < titleList.count(); i++) {
            QStandardItem *item = new QStandardItem(titleList[i]);
            m_dailyModel->setHorizontalHeaderItem(i, item);
    //        delete
        }
        ui->tableView->setModel(m_dailyModel);
        ui->tableView->resizeColumnsToContents();
        this->setEnabled(false);
        QApplication::setOverrideCursor(Qt::WaitCursor);
}

void MainWindow::onListWidgetItemClicked(QListWidgetItem* item) {
    bool isItemChecked = item->checkState();
    if (isItemChecked)
        item->setCheckState(Qt::Unchecked);
    else
        item->setCheckState(Qt::Checked);
}

void MainWindow::upDateCalendars(QList<QDate> dateOfReportList) {
    if (dateOfReportList.isEmpty()){return;}
    ui->calendarWidget->updatesEnabled();
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setBackground(Qt::yellow);
    keywordFormat.setFontWeight(QFont::Bold);

    for (int i = 0; i < dateOfReportList.count(); i++) {
        ui->calendarWidget->setDateTextFormat(dateOfReportList[i], keywordFormat);
        fromCalendarWidget->setDateTextFormat(dateOfReportList[i], keywordFormat);
        toCalendarWidget->setDateTextFormat(dateOfReportList[i], keywordFormat);
    }

    ui->calendarWidget->setSelectedDate(dateOfReportList.last());
    fromCalendarWidget->setSelectedDate(dateOfReportList.last());
    toCalendarWidget->setSelectedDate(dateOfReportList.last());
}


void MainWindow::onBrowseClicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Warning!", "The current application will close. Do you want to continue? If not, press no and print your reports.",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          QString path = mAppController->getBrowsedPath(this);
          if (path == "cancel") return;

          QList<QDate> dateOfReportList1 = mAppController->loadAppleReportFiles(path, this);
          if (dateOfReportList1.count() < 1) return;

          QSettings settings("ADRM");
          settings.setValue("dailyReportsDirPath", path);
          ui->sourceDirLabel->setText(path);

          QMessageBox::warning( this, "Warning", "The application is going to close in order to apply the change. Please run it again." );
          this->close();
      } else {
        return;
      }
}

void MainWindow::onDateClicked(QDate date) {
    clearTable(ui->tableView);
    if (mAppController->mAllDatesList.isEmpty()) {
        QMessageBox::warning( this, "Warning",
                              "There are no daily report files loaded. Please, browse the directory that contains them." );
        return;
    }

    // Checks if there are Daily Report Files for the selected date.
    if(!mAppController->isDailyReport(date)) {
        QMessageBox::warning( this, "Warning", "You should select only the yellow dates." );
        return;
    }

    int numberOfRows = mAppController->onDateClicked(date);
    populateTable(numberOfRows);
}

void MainWindow::clearTable(QTableView* table) {
    while (table->model()->rowCount() > 0) {
        table->model()->removeRow(0);
    }
    table->resizeColumnsToContents();
}

void MainWindow::clearCalendars() {
    if (mAppController->mAllDatesList.isEmpty()){return;}
    ui->calendarWidget->updatesEnabled();
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::black);
    keywordFormat.setBackground(Qt::white);
    keywordFormat.setFontWeight(QFont::Normal);

    for (int i = 0; i < mAppController->mAllDatesList.count(); i++) {
        ui->calendarWidget->setDateTextFormat(mAppController->mAllDatesList[i], keywordFormat);
        fromCalendarWidget->setDateTextFormat(mAppController->mAllDatesList[i], keywordFormat);
        toCalendarWidget->setDateTextFormat(mAppController->mAllDatesList[i], keywordFormat);
    }
}

void MainWindow::populateTable(int j) {
    DailyReport *report = mAppController->mDailyReport;
    populateTableWithList((report->providerList), 0, j);
    populateTableWithList((report->providerCountryList), 1, j);
    populateTableWithList((report->skuList), 2, j);
    populateTableWithList((report->developerList), 3, j);
    populateTableWithList((report->titleList), 4, j);
    populateTableWithList((report->versionList), 5, j);
    populateTableWithList((report->productTypeIdentifierList), 6, j);
    populateTableWithVectorOfInts((report->unitsList), 7, j);
    populateTableWithVectorOfFloats((report->developerProceedsList), 8, j);
    populateTableWithList((report->beginDateList), 9, j);
    populateTableWithList((report->endDateList), 10, j);
    populateTableWithList((report->customerCurrencyList), 11, j);
    populateTableWithList((report->countryCodeList), 12, j);
    populateTableWithList((report->currencyOfProceedsList), 13, j);
    populateTableWithVectorOfInts((report->appleIdentifierList), 14, j);
    populateTableWithVectorOfFloats((report->customerPriceList), 15, j);
    populateTableWithList((report->authorNameList), 16, j);
    //populateTableWithList((report->parentIdentifierList), 17, j);
    delete report;
}

void MainWindow::populateTableWithList(QStringList list, int indexOfColumn, int numberOfRows) {
    for(int i = 0; i < numberOfRows ; i++) {
        m_dailyModel->setItem(i, indexOfColumn, new QStandardItem(list[i]));
    }
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::populateTableWithVectorOfInts(QVector <int> vector, int indexOfColumn, int numberOfRows) {
    for(int i = 0; i < numberOfRows ; i++) {
        m_dailyModel->setItem(i, indexOfColumn, new QStandardItem(QString::number(vector[i])));
    }
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::populateTableWithVectorOfFloats(QVector <float> vector, int indexOfColumn, int numberOfRows) {
    for(int i = 0; i < numberOfRows ; i++)
    {
        m_dailyModel->setItem(i, indexOfColumn, new QStandardItem(QString::number(vector[i])));

    }
    ui->tableView->resizeColumnsToContents();
}

void MainWindow::onDoneClicked() {
    this->setDisabled(true);
    QApplication::setOverrideCursor(Qt::WaitCursor);
//    if (ui->comboBox->currentIndex() == 0)
//    {
//        this->setDisabled(false);
//        QApplication::restoreOverrideCursor();
//        QMessageBox::warning( this, "Warning", "Please select a type of report." );
//        QApplication::setOverrideCursor(Qt::WaitCursor);
//        return;
//    }

    QDate sinceDate = fromCalendarWidget->selectedDate();
    QDate untilDate = toCalendarWidget->selectedDate();
    QString message = "xx"; // = new QList <QDate>;

    ui->headerTextEdit->clear();
    ui->tableTextEdit->clear();
    QStringList authorsSelectedList = populateAuthorsSelectedList();

    QMap <QString, QList<SaleItem*>* > saleItemPerAuthorMap;
    mAppController->populateSaleItemsPerAuthorMap(&saleItemPerAuthorMap, this, sinceDate, untilDate, authorsSelectedList, &message);

    if(saleItemPerAuthorMap.isEmpty())
    {
        saleItemPerAuthorMap.clear();
        this->setDisabled(false);
        QApplication::restoreOverrideCursor();
        return;
    }

    if (ui->comboBox->currentText() == "Balance Sheet")
    {
        foreach (QString key, saleItemPerAuthorMap.keys())
        {
            qSort((saleItemPerAuthorMap[key])->begin(), (saleItemPerAuthorMap[key])->end(), authorLessThan );
        }
        populateReportSheet(sinceDate, untilDate, message, saleItemPerAuthorMap);
    }
    else if (ui->comboBox->currentText() == "Subsidiary Ledger")
    {
        populateSubsidiaryLedgerRep(sinceDate, untilDate, message, saleItemPerAuthorMap);
    }
    else if (ui->comboBox->currentText() == "Design by Country")
    {
        foreach (QString key, saleItemPerAuthorMap.keys())
        {
            qSort((saleItemPerAuthorMap[key])->begin(), (saleItemPerAuthorMap[key])->end(), countryLessThan );
        }
        populateByCountryRep(sinceDate, untilDate, message, saleItemPerAuthorMap);
    }
    saleItemPerAuthorMap.clear();
    this->setDisabled(false);
    QApplication::restoreOverrideCursor();
}

QStringList MainWindow::populateAuthorsSelectedList()
{
    QStringList authorsSelectedList;
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState())
        {
            authorsSelectedList << ui->listWidget->item(i)->text();
        }
    }
    return authorsSelectedList;
}

void MainWindow::populateReportSheet(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* > saleItemPerAuthorMap)
{
    QString previousDesignName, previousParent, previousCurrency, name, parent, currency;
    int numOfItems = 0;
    int numOfItemsPerCurrency = 0;
    int numOfItemsPerDesign = 0;
    int numOfItemsTotal = 0;

    float moneyOfItems = 0;
    float moneyOfItemsPerCurrency = 0;

    float eurosOfItems = 0;
    float eurosOfItemsPerCurrency = 0;
    float eurosOfItemsPerDesign = 0;
    float eurosOfItemsTotal = 0;

    float wingsPercentage = 0;
    float prevWingsPercentage = 0;

    float appleRevenuePerCurrnecy =0; float wingsRevenuePerCurrnecy = 0; float designerRevenuePerCurrnecy = 0;
    float appleRevenuePerDesign = 0; float wingsRevenuePerDesign = 0; float designerRevenuePerDesign = 0;
    float appleRevenueTotal = 0; float wingsRevenueTotal = 0; float designerRevenueTotal = 0;

    balanceSheetRep = new BalanceSheetRep;
    balanceSheetRep->fromDate = fromDate.toString();
    balanceSheetRep->toDate = toDate.toString();
    balanceSheetRep->synthesizeSecondPartOfText();

    foreach (QString key, saleItemPerAuthorMap.keys())
    {
        QList<SaleItem*>* allSAleItemsList = saleItemPerAuthorMap[key];
        if (allSAleItemsList->isEmpty()){continue;}

        balanceSheetRep->author = key;

        previousCurrency = (*allSAleItemsList)[0]->currencyOfProceeds;
        previousDesignName = (*allSAleItemsList)[0]->realName;
        previousParent = (*allSAleItemsList)[0]->parentIdentifier;
        numOfItems = (*allSAleItemsList)[0]->units.toInt();
        moneyOfItems = (*allSAleItemsList)[0]->developerProceeds;
        eurosOfItems = (*allSAleItemsList)[0]->developerProceedsinEuros;
        prevWingsPercentage = (*allSAleItemsList)[0]->wingsPercentage;
        numOfItemsPerCurrency = moneyOfItemsPerCurrency = eurosOfItemsPerCurrency = appleRevenuePerCurrnecy
                = wingsRevenuePerCurrnecy = designerRevenuePerCurrnecy = 0;
        numOfItemsPerDesign = eurosOfItemsPerDesign = appleRevenuePerDesign = wingsRevenuePerDesign = designerRevenuePerDesign = 0;

        for (int i = 0; i < allSAleItemsList->count(); i++)
        {
            currency = (*allSAleItemsList)[i]->currencyOfProceeds;
            name = (*allSAleItemsList)[i]->realName;
            parent = (*allSAleItemsList)[i]->parentIdentifier;
            numOfItems = (*allSAleItemsList)[i]->units.toInt();
            moneyOfItems = (*allSAleItemsList)[i]->developerProceeds;
            eurosOfItems = (*allSAleItemsList)[i]->developerProceedsinEuros;
            if (key == "Wings Systems") { wingsPercentage = 0.7f; }
            else { wingsPercentage = (*allSAleItemsList)[i]->wingsPercentage; }
            //qDebug() << name << wingsPercentage;

            qDebug() << "name: " << name << ", numOfItems: " << numOfItems;

            if (name == previousDesignName && currency == previousCurrency)
            {
                numOfItemsPerCurrency += numOfItems;
                numOfItemsPerDesign += numOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsPerCurrency += eurosOfItems;
                eurosOfItemsPerDesign += eurosOfItems;
                eurosOfItemsTotal += eurosOfItems;
                moneyOfItemsPerCurrency += moneyOfItems;
                previousCurrency = currency;
                previousDesignName = name;
                previousParent = parent;
                prevWingsPercentage = wingsPercentage;
                appleRevenuePerCurrnecy += eurosOfItems * 0.7;
                wingsRevenuePerCurrnecy += eurosOfItems * wingsPercentage;
                designerRevenuePerCurrnecy += eurosOfItems * (0.7 - wingsPercentage);
                appleRevenuePerDesign += eurosOfItems * 0.7;
                wingsRevenuePerDesign += eurosOfItems * wingsPercentage;
                designerRevenuePerDesign += eurosOfItems * (0.7 - wingsPercentage);
                appleRevenueTotal += eurosOfItems * 0.7;
                wingsRevenueTotal += eurosOfItems * wingsPercentage;
                designerRevenueTotal += eurosOfItems * (0.7 - wingsPercentage);
            }
            else if (name == previousDesignName && currency != previousCurrency)
            {
                if (!ui->minReportsCheckBox->isChecked())
                balanceSheetRep->appendLineinTable(previousDesignName, previousParent, numOfItemsPerCurrency, moneyOfItemsPerCurrency, previousCurrency, eurosOfItemsPerCurrency, prevWingsPercentage);
                numOfItemsPerDesign += numOfItems;
                numOfItemsPerCurrency = numOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsPerCurrency = eurosOfItems;
                eurosOfItemsPerDesign += eurosOfItems;
                eurosOfItemsTotal += eurosOfItems;
                moneyOfItemsPerCurrency = moneyOfItems;
                previousCurrency = currency;
                previousDesignName = name;
                previousParent = parent;
                prevWingsPercentage = wingsPercentage;
                appleRevenuePerCurrnecy = eurosOfItems * 0.7;
                wingsRevenuePerCurrnecy = eurosOfItems * wingsPercentage;
                designerRevenuePerCurrnecy = eurosOfItems * (0.7 - wingsPercentage);
                appleRevenuePerDesign += eurosOfItems * 0.7;
                wingsRevenuePerDesign += eurosOfItems * wingsPercentage;
                designerRevenuePerDesign += eurosOfItems * (0.7 - wingsPercentage);
                appleRevenueTotal += eurosOfItems * 0.7;
                wingsRevenueTotal += eurosOfItems * wingsPercentage;
                designerRevenueTotal += eurosOfItems * (0.7 - wingsPercentage);
            }
            else
            {
                if (!ui->minReportsCheckBox->isChecked())
                balanceSheetRep->appendLineinTable(previousDesignName, previousParent, numOfItemsPerCurrency, moneyOfItemsPerCurrency, previousCurrency, eurosOfItemsPerCurrency, prevWingsPercentage);
                balanceSheetRep->appendTotalPerDesignInTable(previousDesignName, previousParent, numOfItemsPerDesign, eurosOfItemsPerDesign, prevWingsPercentage);
                numOfItemsPerDesign = numOfItems;
                numOfItemsPerCurrency = numOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsPerCurrency = eurosOfItems;
                eurosOfItemsPerDesign = eurosOfItems;
                eurosOfItemsTotal += eurosOfItems;
                moneyOfItemsPerCurrency = moneyOfItems;
                previousCurrency = currency;
                previousDesignName = name;
                previousParent = parent;
                prevWingsPercentage = wingsPercentage;
                appleRevenuePerCurrnecy = eurosOfItems * 0.7;
                wingsRevenuePerCurrnecy = eurosOfItems * wingsPercentage;
                designerRevenuePerCurrnecy = eurosOfItems * (0.7 - wingsPercentage);
                appleRevenuePerDesign = eurosOfItems * 0.7;
                wingsRevenuePerDesign = eurosOfItems * wingsPercentage;
                designerRevenuePerDesign = eurosOfItems * (0.7 - wingsPercentage);
                appleRevenueTotal += eurosOfItems * 0.7;
                wingsRevenueTotal += eurosOfItems * wingsPercentage;
                designerRevenueTotal += eurosOfItems * (0.7 - wingsPercentage);
            }

            //Check if the Revenue is negative and append to Comments
            if ((*allSAleItemsList)[i]->customerPrice < 0)
            {
                //qDebug() << "diavasma" << (*allSAleItemsList)[i]->customerPrice;
                message.append(previousDesignName + " on " + (*allSAleItemsList)[i]->date.toString("dd/MM/yyyy") + ", ");
            }
        }
        if (!ui->minReportsCheckBox->isChecked())
        balanceSheetRep->appendLineinTable(previousDesignName, previousParent, numOfItemsPerCurrency, moneyOfItemsPerCurrency, previousCurrency, eurosOfItemsPerCurrency, prevWingsPercentage);
        balanceSheetRep->appendTotalPerDesignInTable(previousDesignName, previousParent, numOfItemsPerDesign, eurosOfItemsPerDesign, prevWingsPercentage);
    }
    balanceSheetRep->appendTotalOfReportInTable(numOfItemsTotal, eurosOfItemsTotal, wingsRevenueTotal, designerRevenueTotal);
    balanceSheetRep->appendFinalPartOfText();

    if(message.left(5) != "There")
    {
        if (!message.isEmpty())
        {
            message = message.left(message.length() - 2);
            message.append(" have negative revenues.");
        }
    }

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState())
        {
            balanceSheetRep->allAuthors.append(ui->listWidget->item(i)->text() + ", ");
        }
    }
    balanceSheetRep->allAuthors = balanceSheetRep->allAuthors.left(balanceSheetRep->allAuthors.length() - 2);

    balanceSheetRep->comments = message;
    //qDebug() << balanceSheetRep->comments;
    balanceSheetRep->synthesizeFirstPartOfText();



    ui->headerTextEdit->setHtml(balanceSheetRep->mHtmlText0 + balanceSheetRep->mHtmlText1);
    ui->tableTextEdit->setHtml(balanceSheetRep->mHtmlText2);

    delete balanceSheetRep;
}

void MainWindow::populateSubsidiaryLedgerRep(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* > saleItemPerAuthorMap)
{
    QString currency, name, parent, dateString, previousDateString;
    QDate date, previousDate;
    int numOfItems = 0;
    int numOfItemsPerDate = 0;
    int numOfItemsTotal = 0;

    float moneyOfItems = 0;

    float eurosOfItems = 0;
    float eurosOfItemsPerDate = 0;
    float eurosOfItemsTotal = 0;

    float appleRevenue = 0; float wingsRevenue = 0; float designerRevenue = 0;
    float appleRevenueTotal = 0; float wingsRevenueTotal = 0; float designerRevenueTotal = 0;

    float wingsPercentage = 0;

    subsidiaryLedgerRep = new SubsidiaryLedgerRep;
    subsidiaryLedgerRep->fromDate = fromDate.toString();
    subsidiaryLedgerRep->toDate = toDate.toString();
    subsidiaryLedgerRep->synthesizeSecondPartOfText();

    foreach (QString key, saleItemPerAuthorMap.keys())
    {
        QList<SaleItem*>* allSAleItemsList = saleItemPerAuthorMap[key];
        if (allSAleItemsList->isEmpty()){continue;}

        subsidiaryLedgerRep->author = key;

        previousDate = (*allSAleItemsList)[0]->date;
        numOfItemsPerDate = eurosOfItemsPerDate = appleRevenue  = wingsRevenue = designerRevenue = 0;

        for (int i = 0; i < allSAleItemsList->count(); i++)
        {
            date = (*allSAleItemsList)[i]->date;
            currency = (*allSAleItemsList)[i]->currencyOfProceeds;
            name = (*allSAleItemsList)[i]->realName;
            parent = (*allSAleItemsList)[i]->parentIdentifier;
            numOfItems = (*allSAleItemsList)[i]->units.toInt();
            moneyOfItems = (*allSAleItemsList)[i]->developerProceeds;
            eurosOfItems = (*allSAleItemsList)[i]->developerProceedsinEuros;
            if (key == "Wings Systems") { wingsPercentage = 0.7f; }
            else { wingsPercentage = (*allSAleItemsList)[i]->wingsPercentage; }

            //qDebug() << wingsPercentage;
            if (date == previousDate)
            {
                if (!ui->minReportsCheckBox->isChecked())
                subsidiaryLedgerRep->appendLineinTable(previousDate.toString("dd/MM/yyyy"), name, parent, numOfItems, moneyOfItems, currency, eurosOfItems, wingsPercentage);
                numOfItemsPerDate += numOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsPerDate += eurosOfItems;
                eurosOfItemsTotal += eurosOfItems;
                appleRevenue += eurosOfItems * 0.7;
                wingsRevenue += eurosOfItems * wingsPercentage;
                designerRevenue += eurosOfItems * (0.7 - wingsPercentage);
                appleRevenueTotal += eurosOfItems * 0.3;
                wingsRevenueTotal += eurosOfItems * wingsPercentage;
                designerRevenueTotal += eurosOfItems * (0.7 - wingsPercentage);
            }
            else
            {
                dateString = date.toString("dd/MM/yyyy");
                previousDateString = previousDate.toString("dd/MM/yyyy");
                subsidiaryLedgerRep->appendTotalPerDateInTable(previousDateString, numOfItemsPerDate, eurosOfItemsPerDate, appleRevenue, wingsRevenue, designerRevenue);
                numOfItemsPerDate = numOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsPerDate = eurosOfItems;
                eurosOfItemsTotal += eurosOfItems;
                appleRevenue = eurosOfItems * 0.7;
                wingsRevenue = eurosOfItems * wingsPercentage;
                designerRevenue = eurosOfItems * (0.7 - wingsPercentage);
                appleRevenueTotal += eurosOfItems * 0.3;
                wingsRevenueTotal += eurosOfItems * wingsPercentage;
                designerRevenueTotal += eurosOfItems * (0.7 - wingsPercentage);
                if (!ui->minReportsCheckBox->isChecked())
                subsidiaryLedgerRep->appendLineinTable(dateString, name, parent, numOfItemsPerDate, moneyOfItems, currency, eurosOfItems, wingsPercentage);
            }
            previousDate = date;

            //Check if the Revenue is negative and append to Comments
            if ((*allSAleItemsList)[i]->customerPrice < 0)
            {
//                qDebug() << "customerPrice: " << (*allSAleItemsList)[i]->customerPrice;
//                qDebug() << "developerProceeds: " << (*allSAleItemsList)[i]->developerProceeds;
                message.append(name + " on " + (*allSAleItemsList)[i]->date.toString("dd/MM/yyyy") + ", ");
            }
        }
        subsidiaryLedgerRep->appendTotalPerDateInTable(previousDate.toString("dd/MM/yyyy"), numOfItemsPerDate, eurosOfItemsPerDate, appleRevenue, wingsRevenue, designerRevenue);
    }
    subsidiaryLedgerRep->appendTotalOfReportInTable(numOfItemsTotal, eurosOfItemsTotal, appleRevenueTotal, wingsRevenueTotal, designerRevenueTotal);
    subsidiaryLedgerRep->appendFinalPartOfText();

    if(message.left(5) != "There")
    {
        if (!message.isEmpty())
        {
            message = message.left(message.length() - 2);
            message.append(" have negative revenues.");
        }
    }
    subsidiaryLedgerRep->comments = message;

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState())
        {
            subsidiaryLedgerRep->allAuthors.append(ui->listWidget->item(i)->text() + ", ");
        }
    }
    subsidiaryLedgerRep->allAuthors = subsidiaryLedgerRep->allAuthors.left(subsidiaryLedgerRep->allAuthors.length() - 2);

    subsidiaryLedgerRep->synthesizeFirstPartOfText();

    ui->headerTextEdit->setHtml(subsidiaryLedgerRep->mHtmlText0 + subsidiaryLedgerRep->mHtmlText1);
    ui->tableTextEdit->setHtml(subsidiaryLedgerRep->mHtmlText2);

    delete subsidiaryLedgerRep;
}

void MainWindow::populateByCountryRep(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* > saleItemPerAuthorMap)
{
    QString country, name, previousName, previousCountry;

    int numOfItems = 0;
    int numOfItemsPerCountry = 0;
    int numOfItemsPerDesign = 0;
    int numOfItemsTotal = 0;

    float moneyOfItems = 0;
    float moneyOfItemsPerCountry = 0;

    float eurosOfItems = 0;
    float eurosOfItemsPerCountry = 0;
    float eurosOfItemsPerDesign = 0;
    float eurosOfItemsTotal = 0;
    float wingsRevenueTotal = 0;
    float designerRevenueTotal = 0;

    float wingsPercentage = 0;
    float prevWingsPercentage = 0;

    byCountryRep = new ByCountryRep;
    byCountryRep->fromDate = fromDate.toString();
    byCountryRep->toDate = toDate.toString();
    byCountryRep->synthesizeSecondPartOfText();

    foreach (QString key, saleItemPerAuthorMap.keys())
    {
        QList<SaleItem*>* allSAleItemsList = saleItemPerAuthorMap[key];
        if (allSAleItemsList->isEmpty()){continue;}

        byCountryRep->author = key;

        previousName = (*allSAleItemsList)[0]->realName;
        if (key == "Wings Systems") { prevWingsPercentage = 0.7f; }
        else { prevWingsPercentage = (*allSAleItemsList)[0]->wingsPercentage; }
        previousCountry = (*allSAleItemsList)[0]->countryCode;
        numOfItemsPerCountry = eurosOfItemsPerCountry = 0;
        numOfItemsPerDesign = eurosOfItemsPerDesign = 0;

        for (int i = 0; i < allSAleItemsList->count(); i++)
        {
            name = (*allSAleItemsList)[i]->realName;
            country = (*allSAleItemsList)[i]->countryCode;
            numOfItems = (*allSAleItemsList)[i]->units.toInt();
            moneyOfItems = (*allSAleItemsList)[i]->developerProceeds;
            eurosOfItems = (*allSAleItemsList)[i]->developerProceedsinEuros;
            if (key == "Wings Systems") { wingsPercentage = 0.7f; }
            else { wingsPercentage = (*allSAleItemsList)[i]->wingsPercentage; }

            if (name == previousName && country ==previousCountry)
            {
                numOfItemsPerCountry += numOfItems;
                moneyOfItemsPerCountry += moneyOfItems;
                eurosOfItemsPerCountry += eurosOfItems;
                numOfItemsPerDesign += numOfItems;
                eurosOfItemsPerDesign += eurosOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsTotal += eurosOfItems;
                wingsRevenueTotal += wingsPercentage * eurosOfItems;
                designerRevenueTotal += (0.7 - wingsPercentage) * eurosOfItems;
            }
            else if (name == previousName && country !=previousCountry)
            {
                if (country == previousCountry)
                {
                    if (!ui->minReportsCheckBox->isChecked())
                    byCountryRep->appendLineinTable(previousName, previousCountry, numOfItemsPerCountry, eurosOfItemsPerCountry, prevWingsPercentage);
                }
                else
                {
                    if (!ui->minReportsCheckBox->isChecked())
                    byCountryRep->appendLineinTable(previousName, previousCountry, numOfItemsPerCountry, eurosOfItemsPerCountry, prevWingsPercentage);
                }
                numOfItemsPerCountry = numOfItems;
                eurosOfItemsPerCountry = eurosOfItems;
                numOfItemsPerDesign += numOfItems;
                moneyOfItemsPerCountry += moneyOfItems;
                eurosOfItemsPerDesign += eurosOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsTotal += eurosOfItems;
                wingsRevenueTotal += wingsPercentage * eurosOfItems;
                designerRevenueTotal += (0.7 - wingsPercentage) * eurosOfItems;
            }
            else
            {
                if (!ui->minReportsCheckBox->isChecked())
                byCountryRep->appendLineinTable(previousName, previousCountry, numOfItemsPerCountry, eurosOfItemsPerCountry, prevWingsPercentage);
                byCountryRep->appendTotalPerDesignInTable(previousName, numOfItemsPerDesign, eurosOfItemsPerDesign, prevWingsPercentage);
                numOfItemsPerCountry = numOfItems;
                eurosOfItemsPerCountry = eurosOfItems;
                numOfItemsPerDesign = numOfItems;
                moneyOfItemsPerCountry = moneyOfItems;
                eurosOfItemsPerDesign = eurosOfItems;
                numOfItemsTotal += numOfItems;
                eurosOfItemsTotal += eurosOfItems;
                wingsRevenueTotal += wingsPercentage * eurosOfItems;
                designerRevenueTotal += (0.7 - wingsPercentage) * eurosOfItems;
            }
            previousCountry = country;
            previousName = name;
            prevWingsPercentage = wingsPercentage;

            //Check if the Revenue is negative and append to Comments
            if ((*allSAleItemsList)[i]->customerPrice < 0)
            {
                //qDebug() << "diavasma" << (*allSAleItemsList)[i]->customerPrice;
                message.append(previousName + " on " + (*allSAleItemsList)[i]->date.toString("dd/MM/yyyy") + ", ");
            }
        }
        if (!ui->minReportsCheckBox->isChecked())
        byCountryRep->appendLineinTable(previousName, previousCountry, numOfItemsPerCountry, eurosOfItemsPerCountry, prevWingsPercentage);
        byCountryRep->appendTotalPerDesignInTable(name, numOfItemsPerDesign, eurosOfItemsPerDesign, prevWingsPercentage);
    }
    byCountryRep->appendTotalOfReportInTable(numOfItemsTotal, eurosOfItemsTotal, wingsRevenueTotal, designerRevenueTotal);
    byCountryRep->appendFinalPartOfText();

    if(message.left(5) != "There")
    {
        if (!message.isEmpty())
        {
            message = message.left(message.length() - 2);
            message.append(" have negative revenues.");
        }
    }

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if (ui->listWidget->item(i)->checkState())
        {
            byCountryRep->allAuthors.append(ui->listWidget->item(i)->text() + ", ");
        }
    }
    byCountryRep->allAuthors = byCountryRep->allAuthors.left(byCountryRep->allAuthors.length() - 2);

    byCountryRep->comments = message;
    byCountryRep->synthesizeFirstPartOfText();

    ui->headerTextEdit->setHtml(byCountryRep->mHtmlText0 + byCountryRep->mHtmlText1);
    ui->tableTextEdit->setHtml(byCountryRep->mHtmlText2);

    delete byCountryRep;
}

void MainWindow::onAuthorsDesignsRespReceived(QStringList *list) {
    // Populate the Authors' CheckList.
    for (int i = 0; i < list->count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setData( Qt::DisplayRole, (*list)[i] );
        item->setData( Qt::CheckStateRole, Qt::Checked );
        ui->listWidget->addItem(item);
    }
    ui->listWidget->item(0)->setCheckState(Qt::Unchecked);

    QString dailyReportsPath = mAppController->dailyReportsDirPath();

    ui->sourceDirLabel->setText(dailyReportsPath);
    QList<QDate> dateOfReportList = mAppController->loadAppleReportFiles(dailyReportsPath, this);
    upDateCalendars(dateOfReportList);
    if (mAppController->mAllDatesList.isEmpty()){return;}
    onDateClicked(dateOfReportList.last());

    // Enable window
    this->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void MainWindow::onExportReportClicked()
{
    QFileDialog fileDialog(this, "Save file", QDir::currentPath(), ".html");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setWindowModality(Qt::ApplicationModal);
    int result = fileDialog.exec();
    if (result == 1)
    {
        QString filename  = fileDialog.selectedFiles().first();
        filename.append(".html");
        QFile f( filename );
        f.open( QIODevice::WriteOnly );
        QTextStream outStream(&f);

        QString htmlString1 = ui->headerTextEdit->document()->toHtml();
        htmlString1 = htmlString1.left(htmlString1.length() - 14);
        outStream << htmlString1;
        //qDebug() << htmlString1;

        QString htmlString2 = ui->tableTextEdit->document()->toHtml();
        htmlString2 = htmlString2.right(htmlString2.length() - 94);
        outStream << htmlString2;
        //qDebug() << htmlString2;

        f.close();
    }
    else {return;}
}

void MainWindow::onQuitClicked()
{
    this->close();
}
