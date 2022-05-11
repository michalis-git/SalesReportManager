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

    QStringList populateAuthorsSelectedList();
    void upDateCalendars(QList<QDate> dateOfReportList);
    void clearCalendars();



private slots:
    void onDateClicked(QDate date);
    void onDoneClicked();
    void onExportReportClicked();
    void onQuitClicked();
    void onListWidgetItemClicked(QListWidgetItem*item);
    void onActionPreferencesTriggered();
};

#endif // MAINWINDOW_H
