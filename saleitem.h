#ifndef SALEITEM_H
#define SALEITEM_H

#include <QString>
#include <QDate>

class SaleItem
{
public:
    SaleItem();
    ~SaleItem();
    QString provider, providerCountry, sku, developer, title,
            version, productTypeIdentifier, units, beginDate,
            endDate, customerCurrency, countryCode, currencyOfProceeds, appleIdentifier,
            author, parentIdentifier, realName;
    QDate date;
    float developerProceeds;
    float developerProceedsinEuros;
    float companyPercentage;
    float customerPrice;
};

#endif // SALEITEM_H
