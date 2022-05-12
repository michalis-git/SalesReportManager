#ifndef PRODUCTSBYCOUNTRY_H
#define PRODUCTSBYCOUNTRY_H

#include "financialreport.h"

#include <purchases.h>

class QStandardItemModel;
class QStandardItem;

typedef QList<Purchase> PurchasesList;
typedef QMap<QString, PurchasesList> CountryPurchasesMap;

class ProductsByCountry :public FinancialReport
{
    QMap<QString, CountryPurchasesMap> m_map;

    void appendLineToModel(const QString &vHeader, const QString &title, const QString &country,
                           const int &numberOfItems, const float &valueOfItems, double applePercentage, const float &appleRevenue, double companyPercentage, const float &companyRevenue) const;

public:
    ProductsByCountry(const Purchases &purchases);
    QStandardItemModel *getModel() const;
};

#endif // PRODUCTSBYCOUNTRY_H
