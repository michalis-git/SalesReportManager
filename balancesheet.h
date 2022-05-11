#ifndef BALANCESHEET_H
#define BALANCESHEET_H

#include "financialreport.h"
#include <purchases.h>

class QStandardItemModel;
class QStandardItem;


typedef QList<Purchase> PurchasesList;
class BalanceSheet : public FinancialReport
{
    QMap<QString, PurchasesList> m_map;

    void appendLineToModel(const QString &vHeader, const QString &title,
                           const int &numberOfItems, const float &valueInCurrency, const QString &currency, const float &valueInBaseCurrency, double applePercentage, const float &appleRevenue, double companyPercentage, const float &companyRevenue) const;

public:
    BalanceSheet(const Purchases &purchases);
    QStandardItemModel *getModel() const override;
};

#endif // BALANCESHEET_H
