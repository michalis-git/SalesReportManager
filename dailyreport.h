#ifndef DAILYREPORT_H
#define DAILYREPORT_H

#include <QVector>
#include <QStringList>

class QDate;
class QString;

class DailyReport
{
public:
    DailyReport();
    ~DailyReport();
    QDate* date;
    QStringList providerList;
    QStringList providerCountryList;
    QStringList skuList;
    QStringList realNameList;
    QStringList authorNameList;
    QStringList developerList;
    QStringList titleList;
    QStringList versionList;
    QStringList productTypeIdentifierList;
    QVector<int> unitsList;
    QVector<float> developerProceedsList;
    QStringList beginDateList;
    QStringList endDateList;
    QStringList customerCurrencyList;
    QStringList countryCodeList;
    QStringList currencyOfProceedsList;
    QVector<int> appleIdentifierList;
    QVector<float> customerPriceList;
    QStringList parentIdentifierList;

    void setDate(QDate* myDate);
    QDate* getDate();

};

#endif // DAILYREPORT_H
