#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "qobject.h"
#include "dailyreport.h"
#include <qlist.h>
#include <QMap>

class QStandardItemModel;
class QFile;
class HttpRequest;
class MainWindow;
class QDate;
class SaleItem;

class AppController : public QObject
{
    Q_OBJECT

public:
    AppController(MainWindow* mainwindow);
    ~AppController();
    MainWindow *m_mainWindow;
    HttpRequest *request, *artisticRequest, *request1, *exchangeRatesRequest;
    QString mDirectory, mCommentString;
    DailyReport *mDailyReport;
    QString m_ratesDirPath;
    QString m_dailyReportsDirPath;
    QList <QDate> mAllDatesList;
    QStringList mAllCurrenciesList;

    QList <QDate> readFilesFromDirectory(MainWindow *mainwindow, QString path);
    QList <QDate> onBrowseClicked(MainWindow *mainwindow);

//    QList <SaleItem*> onBalanceSheetSelected(MainWindow* mainwindow, QDate sinceDate, QDate untilDate, QString *missingDatesMessage);
    void populateSaleItemsPerAuthorMap(QMap <QString, QList <SaleItem*>* > *saleItemsPerAuthorList, MainWindow* mainwindow, QDate sinceDate, QDate untilDate, QStringList authorsSelectedList, QString *missingDatesMessage);
    void onSubsidiarySheetSelected(QMap <QString, QList <SaleItem*>* > *saleItemsPerAuthorList, MainWindow* mainwindow, QDate sinceDate, QDate untilDate, QStringList authorsSelectedList, QString *missingDatesMessage);

    QStringList browseSourceDir();
//    QStringList getDatesOfReports(QStringList fullPathList);
    QDate stringToDate(QString dateStr);
    QList <QDate> getDatesOfReports(QStringList list);
    QString getMissingDates(QList <QDate> list);
//    QString getMissingDates1(QDate* sinceDate, QDate* untilDate);
    bool isDailyReport(QDate date);

    QStandardItemModel *dayReportModel(const QDate &date);
    QByteArray gzipDecompress(QByteArray &compressData);
    QString unpackDailyReportFile(QDate date);
    QStringList *getAuthorList();
    QString getBrowsedPath(MainWindow* mainwindow);
    QList<QDate> loadAppleReportFiles(QString path, MainWindow *mainwindow);
    QString dailyReportsDirPath();

private:
//    void populateAllSaleItemList(QList <SaleItem*>* allSaleItemList, QDate *date);
    void populateAllSaleItemList(QMap<QString, QList<SaleItem *> *> *saleItemsPerAuthorMap, QDate *date, MainWindow *mainwindow);
//    void createExchangeRateFiles(QList <QDate> dateOfReportList, MainWindow *mainwindow);
    void createAdditionalDateList(QDate startDate, QDate endDate, MainWindow *mainwindow);
    void getFirstLastDatesFromRateFile(QDate& firstDateOfRates, QDate& lastDateOfRates, QStringList fullPathList, MainWindow *mainwindow);
    void createRateFiles(QList<QDate> dateOfReportList, MainWindow *mainwindow);

private slots:
    void requestIsFinished();
    void artisticRequestIsFinished();

signals:
    void authorsDesignRespReceived(QStringList* list);
    void artisticAuthorsDesignRespReceived(QStringList* list);

};

#endif // APPCONTROLLER_H
