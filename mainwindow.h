#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qdatetime.h"

class DailyReport;
class AppController;
class QStandardItemModel;
class QTableView;
class SaleItem;
class QCalendarWidget;
class QListWidgetItem;

class BalanceSheetRep;
class SubsidiaryLedgerRep;
class ByCountryRep;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    AppController *mAppController;
    QStandardItemModel *m_dailyModel;
    QList<QDate> calendarDateList;
    QCalendarWidget* fromCalendarWidget, *toCalendarWidget;

    BalanceSheetRep *balanceSheetRep;
    SubsidiaryLedgerRep *subsidiaryLedgerRep;
    ByCountryRep *byCountryRep;

    void findMissingDates(QStringList list);
    void populateTable(int j);
    void clearTable(QTableView *table);
    void populateTableWithList(QStringList list, int indexOfColumn, int numberOfRows);
    void populateTableWithVectorOfInts(QVector<int> vector, int indexOfColumn, int numberOfRows);
    void populateTableWithVectorOfFloats(QVector<float> vector, int indexOfColumn, int numberOfRows);

    void populateReportSheet(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* >);
    void populateSubsidiaryLedgerRep(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* >);
    void populateByCountryRep(QDate fromDate, QDate toDate, QString message, QMap <QString, QList<SaleItem*>* >);

    QStringList populateAuthorsSelectedList();
    void upDateCalendars(QList<QDate> dateOfReportList);
    void clearCalendars();
    void initializeTableView();


private slots:
    void onBrowseClicked();
    void onDateClicked(QDate date);
    void onDoneClicked();
    void onAuthorsDesignsRespReceived(QStringList* list);
    void onExportReportClicked();
    void onQuitClicked();
    void onListWidgetItemClicked(QListWidgetItem*item);
};

#endif // MAINWINDOW_H
