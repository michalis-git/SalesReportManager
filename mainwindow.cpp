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
#include "preferencesdialog.h"
#include "appsettings.h"
#include "purchases.h"

#include "QDate"
#include <QList>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextCharFormat>
#include <QSettings>
#include <QTextEdit>
#include <QScrollBar>
#include <QLayoutItem>

#include "balancesheet.h"
#include "productsbycountry.h"
#include "subsidiaryledgerreport.h"

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

//    this->showMaximized();

    connect(ui->calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(onDateClicked(QDate)));
    connect(ui->doneButton, SIGNAL(clicked()), this, SLOT(onDoneClicked()));
    connect(ui->actionExport_Report_as_HTML, SIGNAL(triggered()), this, SLOT(onExportReportClicked()));
    connect(ui->actionActionExport, SIGNAL(triggered()), this, SLOT(onExportReportClicked()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onQuitClicked()));
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListWidgetItemClicked(QListWidgetItem*)));
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::onActionPreferencesTriggered);

    // Initialize Calendars
    ui->fromDateEdit->setDisplayFormat("dd/MM/yyyy");
    ui->fromDateEdit->setCalendarPopup(true);
    fromCalendarWidget = new QCalendarWidget;
    ui->fromDateEdit->setCalendarWidget(fromCalendarWidget);

    ui->toDateEdit->setDisplayFormat("dd/MM/yyyy");
    ui->toDateEdit->setCalendarPopup(true);
    toCalendarWidget = new QCalendarWidget;
    ui->toDateEdit->setCalendarWidget(toCalendarWidget);

    QString dailyReportsPath = AppSettings::instance()->reportsDirPath();

    QList<QDate> dateOfReportList = mAppController->loadAppleReportFiles(dailyReportsPath, this);
    upDateCalendars(dateOfReportList);
    if (mAppController->mAllDatesList.isEmpty()){return;}
    onDateClicked(dateOfReportList.last());
}

MainWindow::~MainWindow() {
    delete ui;
    delete mAppController;
    delete m_dailyModel;
    delete fromCalendarWidget;
    delete toCalendarWidget;
}

void MainWindow::onListWidgetItemClicked(QListWidgetItem* item) {
    bool isItemChecked = item->checkState();
    if (isItemChecked)
        item->setCheckState(Qt::Unchecked);
    else
        item->setCheckState(Qt::Checked);
}

void MainWindow::onActionPreferencesTriggered() {
    PreferencesDialog dialog(this);
    dialog.exec();
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

void MainWindow::onDateClicked(QDate date) {
    if (mAppController->mAllDatesList.isEmpty()) {
        QMessageBox::warning( this, "Warning",
                              "There are no daily report files loaded. Please, browse the directory that contains them." );
        return;
    }

    //TODOS
    // Checks if there are Daily Report Files for the selected date.
    if(!mAppController->isDailyReport(date)) {
        QMessageBox::warning( this, "Warning", "You should select only the yellow dates." );
        return;
    }

    QStandardItemModel *model = mAppController->purchasesModel(date, date);
    ui->dayTableView->setModel(model);
    ui->dayTableView->resizeColumnsToContents();
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

void MainWindow::onDoneClicked() {
    this->setDisabled(true);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QDate fromDate = fromCalendarWidget->selectedDate();
    QDate toDate = toCalendarWidget->selectedDate();
    // Checks if the selected time period is valid.
    if (fromDate > toDate) {
        QMessageBox::warning(this, "Warning", "The date in the first calendar must be earlier than the date in the second one.");
        return;
    }

    QString message = "xx"; // = new QList <QDate>;

    QString currentText = ui->comboBox->currentText();

    Purchases purchases(ui->fromDateEdit->date(), ui->toDateEdit->date());
    FinancialReport *report;
    if (currentText == "Balance Sheet") {
      report = new BalanceSheet(purchases);
    } else if (currentText == "Subsidiary Ledger") {
      report = new SubsidiaryLedgerReport(purchases);
    } else { //(currentText == "Product by Country") {
      report = new ProductsByCountry(purchases);
    }
    ui->reportTableView->setModel(report->getModel());
    ui->reportTableView->resizeColumnsToContents();

    ui->titleLabel->setText(report->title());
    ui->descriptionLabel->setText(report->description());
    ui->startDateLabel->setText(report->startDate().toString("dd-MM-yyyy"));
    ui->endDateLabel->setText(report->endDate().toString("dd-MM-yyyy"));



    this->setDisabled(false);
    QApplication::restoreOverrideCursor();
}

void MainWindow::onExportReportClicked()
{
//    QFileDialog fileDialog(this, "Save file", QDir::currentPath(), ".html");
//    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
//    fileDialog.setWindowModality(Qt::ApplicationModal);
//    int result = fileDialog.exec();
//    if (result == 1)
//    {
//        QString filename  = fileDialog.selectedFiles().first();
//        filename.append(".html");
//        QFile f( filename );
//        f.open( QIODevice::WriteOnly );
//        QTextStream outStream(&f);

//        QString htmlString1 = ui->headerTextEdit->document()->toHtml();
//        htmlString1 = htmlString1.left(htmlString1.length() - 14);
//        outStream << htmlString1;
//        //qDebug() << htmlString1;

//        QString htmlString2 = ui->tableTextEdit->document()->toHtml();
//        htmlString2 = htmlString2.right(htmlString2.length() - 94);
//        outStream << htmlString2;
//        //qDebug() << htmlString2;

//        f.close();
//    }
//    else {return;}
}

void MainWindow::onQuitClicked() {
    this->close();
}
