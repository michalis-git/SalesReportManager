#ifndef PURCHASES_H
#define PURCHASES_H

#include <QMap>
#include "purchase.h"

class QStandardItemModel;



class Purchases
{
    QStandardItemModel *m_purchasesModel;
    QList<Purchase> m_purchaseList;

    QString decompressFile(const QDate &date);

public:
    Purchases(const QDate &fromDate, const QDate &toDate);
    QStandardItemModel *purchasesModel() const;
};

#endif // PURCHASES_H
