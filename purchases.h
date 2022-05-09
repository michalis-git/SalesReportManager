#ifndef PURCHASES_H
#define PURCHASES_H

#include <QMap>
#include "purchase.h"

class QStandardItemModel;



class Purchases
{
    QDate m_startDate, m_endDate;
    QStandardItemModel *m_purchasesModel;
    QList<Purchase> m_purchaseList;

    QString decompressFile(const QDate &date);

public:
    Purchases(const QDate &fromDate, const QDate &toDate);
    QStandardItemModel *purchasesModel() const;
    const QList<Purchase> &purchaseList() const;
    const QDate &startDate() const;
    const QDate &endDate() const;
};

#endif // PURCHASES_H
