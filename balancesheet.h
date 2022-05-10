#ifndef BALANCESHEET_H
#define BALANCESHEET_H

#include <purchases.h>

class QStandardItemModel;
class QStandardItem;


typedef QList<Purchase> PurchasesList;
class BalanceSheet
{
    QString m_title;
    QString m_description;
    QDate m_startDate, m_endDate;
    QMap<QString, PurchasesList> m_map;
    QStandardItemModel *m_model;
    void setHeadersToModel();
    void appendLineToModel(const QString &vHeader, const QString &title, const QString &country,
                           const int &numberOfItems, const float &valueOfItems) const;
    void styleItem(int row, const QString &header, QStandardItem *item) const;
public:
    BalanceSheet(const Purchases &purchases);
    QStandardItemModel *getModel() const;
    const QString &title() const;
    const QString &description() const;
    const QDate &startDate() const;
    const QDate &endDate() const;
};

#endif // BALANCESHEET_H
