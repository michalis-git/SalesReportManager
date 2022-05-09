#ifndef PRODUCTSBYCOUNTRY_H
#define PRODUCTSBYCOUNTRY_H

#include <purchases.h>

class QStandardItemModel;
class QStandardItem;

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
    QString m_title;
    QString m_description;
    QDate m_startDate, m_endDate;
    QMap<QString, CountryPurchasesMap> m_map;
    QStandardItemModel *m_model;
    void setHeadersToModel();
    void appendLineToModel(const QString &vHeader, const QString &title, const QString &country,
                           const int &numberOfItems, const float &valueOfItems) const;
    void styleItem(int row, const QString &header, QStandardItem *item) const;
public:
    ProductsByCountry(const Purchases &purchases);
    QStandardItemModel *getModel() const;
    const QString &title() const;
    const QString &description() const;
    const QDate &startDate() const;
    const QDate &endDate() const;
};

#endif // PRODUCTSBYCOUNTRY_H
