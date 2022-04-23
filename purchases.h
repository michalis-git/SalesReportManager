#ifndef PURCHASES_H
#define PURCHASES_H

#include <QMap>
#include "purchase.h"

class QStandardItemModel;



class Purchases
{
    QStandardItemModel *m_purchasesModel;
    QList<Purchase> m_purchaseList;

public:
    Purchases(const QDate &date, const QString &path);
    Purchases(const QDate &fromDate, const QDate &toDate, const QString &path);
    QStandardItemModel *purchasesModel() const;
};

#endif // PURCHASES_H
