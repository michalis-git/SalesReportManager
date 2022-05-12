#ifndef SUBSIDIARYLEDGERREPORT_H
#define SUBSIDIARYLEDGERREPORT_H

#include "financialreport.h"

class SubsidiaryLedgerReport : public FinancialReport
{
  QMap<QString, PurchasesList> m_map;

  void appendLineToModel(const QString &vHeader, const QString &date, const QString &title,
                         const int &numberOfItems, const float &valueInCurrency, const QString &currency, const float &valueInBaseCurrency, double applePercentage, const float &appleRevenue, double companyPercentage, const float &companyRevenue) const;

public:
  SubsidiaryLedgerReport(const Purchases &purchases);

  QStandardItemModel *getModel() const override;
};

#endif // SUBSIDIARYLEDGERREPORT_H
