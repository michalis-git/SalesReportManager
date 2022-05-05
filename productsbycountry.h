#ifndef PRODUCTSBYCOUNTRY_H
#define PRODUCTSBYCOUNTRY_H

#include <purchases.h>

class QStandardItemModel;

//struct Product {
//    QStringList headers;
//        headers << "Product" << "Country" << "Items"
//        << "Total (€)" << "Apple %" << "Apple Revenue"
//        << "SoftwareHouse %" << "SoftwareHouse Revenue";

//};

typedef QList<Purchase> PurchasesList;
typedef QMap<QString, PurchasesList> CountryPurchasesMap;

class ProductsByCountry
{
    QMap<QString, CountryPurchasesMap> m_map;
    QStandardItemModel *m_model;
    void appendLineToModel(const QString &title, const QString &country,
                           const int &numberOfItems, const float &valueOfItems);
public:
    ProductsByCountry(const Purchases &purchases);
    QStandardItemModel *getModel() const;
};

#endif // PRODUCTSBYCOUNTRY_H
