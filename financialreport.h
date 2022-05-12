#ifndef FINANCIALREPORT_H
#define FINANCIALREPORT_H

#include <QDate>
#include <QString>
#include <purchases.h>



class QStandardItemModel;
class QStandardItem;

typedef QList<Purchase> PurchasesList;

class FinancialReport
{
protected:
  QString m_title;
  QString m_description;
  QDate m_startDate, m_endDate;
  double m_applePercentage;
  double m_companyPercentage;
  QStandardItemModel *m_model;

  void setHeadersToModel(const QStringList &headers);
  void styleItem(const QString &header, QStandardItem *item) const;

public:
  FinancialReport(const Purchases &purchases);
  virtual QStandardItemModel *getModel() const = 0;
  const QString &title() const;
  const QString &description() const;
  const QDate &startDate() const;
  const QDate &endDate() const;
};

#endif // FINANCIALREPORT_H
