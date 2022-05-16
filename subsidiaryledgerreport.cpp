#include "subsidiaryledgerreport.h"

#include <QStandardItem>


SubsidiaryLedgerReport::SubsidiaryLedgerReport(const Purchases &purchases)
    : FinancialReport(purchases) {
  m_title = QObject::tr("Subsidiary Ledger Report");
  m_description = QObject::tr("This report presents all purchases done in the selected time period grouped by date. ");
  m_description.append("The total revenue for this period is in the end of the table. The date format is dd/mm/yyyy.");
  QList<Purchase> purchasesList = purchases.purchaseList();
  for (auto &purchase : purchasesList) {
    QString date = purchase.propertyByName(Property::END_DATE).stringValue();
    if (m_map.contains(date)) {
      PurchasesList purchasesList = m_map.value(date);
      purchasesList << purchase;
      m_map.insert(date, purchasesList);
    } else {
      m_map.insert(date, PurchasesList{ purchase });
    }
  }

  QStringList headers;
  headers << "Date" << "Product" << "Items"
          << "Revenue" << "Currency" << "Revenue  (€)"
          << "Apple %" << "Apple Revenue"
          << "SoftwareHouse %" << "SoftwareHouse Revenue";
  setHeadersToModel(headers);
}


QStandardItemModel *SubsidiaryLedgerReport::getModel() const {  int numberOfItems = 0, totalItemsPerProduct = 0, totalItems = 0;
  float totalValuePerProduct = 0, totalValue = 0;
  QString currency;
  for (auto &date : m_map.keys()) {
    auto purchasesList = m_map.value(date);
    numberOfItems = 0;
    int counter  = 1;
    for (auto &purchase : purchasesList) {
      numberOfItems = 0;
      QString title = purchase.propertyByName(Property::TITLE).stringValue();
      int units = purchase.propertyByName(Property::UNITS).value().toInt();
      currency = purchase.propertyByName(Property::CUSTOMER_CURRENCY).stringValue();
      numberOfItems += units;
      totalItemsPerProduct += units;
      totalItems += units;
      float devProceeds = purchase.propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
      float devProceedsInEuro = purchase.propertyByName(Property::DEVELOPER_PROCEEDS_EUROS).value().toFloat();
      totalValuePerProduct += devProceedsInEuro;
      totalValue += devProceedsInEuro;
      appendLineToModel(QString::number(counter), date, title, numberOfItems, devProceeds, currency, devProceedsInEuro,
                        m_applePercentage, (m_applePercentage / 100) * devProceedsInEuro,
                        m_companyPercentage, (m_companyPercentage / 100) * devProceedsInEuro);
      counter++;
    }
    appendLineToModel("Subtotal", date, "", totalItemsPerProduct, 0,  "", totalValuePerProduct,
                      m_applePercentage, (m_applePercentage / 100) * totalValuePerProduct,
                      m_companyPercentage, (m_companyPercentage / 100) * totalValuePerProduct);
    totalItemsPerProduct = 0;
    totalValuePerProduct = 0;
  }
  appendLineToModel("Total", "", "", totalItems, 0, "", totalValue,
                    m_applePercentage, (m_applePercentage / 100) * totalValue,
                    m_companyPercentage, (m_companyPercentage / 100) * totalValue);
  return m_model;
}

void SubsidiaryLedgerReport::appendLineToModel(const QString &vHeader,
                                               const QString &date, const QString &title, const int &numberOfItems,
                                               const float &valueInCurrency, const QString &currency, const float &valueInBaseCurrency,
                                               double applePercentage, const float &appleRevenue,
                                               double companyPercentage, const float &companyRevenue) const {
  QStringList fields;
  fields << date << title << QString::number(numberOfItems)
         << QString::number((valueInCurrency)) << currency << QString::number((valueInBaseCurrency))
         << QString::number(applePercentage) << QString::number(appleRevenue)
         << QString::number(companyPercentage) << QString::number(companyRevenue);
  int j = 0;
  int row = m_model->rowCount();
  for (auto &field : fields) {
    QStandardItem *item = new QStandardItem(field);
    styleItem(vHeader, item);
    m_model->setItem(row, j, item);
    j++;
  }

  QStandardItem *headerItem = new QStandardItem(vHeader);
  styleItem(vHeader, headerItem);
  m_model->setVerticalHeaderItem(row, headerItem);
}
