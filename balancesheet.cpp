#include "balancesheet.h"

#include <QStandardItem>
#include <QDebug>


BalanceSheet::BalanceSheet(const Purchases &purchases)
    : FinancialReport(purchases) {
  m_title = QObject::tr("Balance Sheet Report");
  m_description = QObject::tr("This report presents all purchases done in the selected time period grouped by product. ");
  m_description.append("The total revenue for this period is in the end of the table. The date format is dd/mm/yyyy.");

  QList<Purchase> purchasesList = purchases.purchaseList();
  for (auto &purchase : purchasesList) {
    QString title = purchase.propertyByName(Property::TITLE).stringValue();
    if (m_map.contains(title)) {
      PurchasesList purchasesList = m_map.value(title);
      purchasesList << purchase;
      m_map.insert(title, purchasesList);
    } else {
      m_map.insert(title, PurchasesList{ purchase });
    }
  }

  QStringList headers;
  headers << "Product" << "Items" << "Revenue"
          << "Currency" << "Revenue €"
          << "Apple %" << "Apple €"
          << "SoftwareHouse %" << "SoftwareHouse €";
  setHeadersToModel(headers);
}

QStandardItemModel *BalanceSheet::getModel() const {
  int numberOfItems = 0, totalItemsPerProduct = 0, totalItems = 0;
  float totalValuePerProduct = 0, totalValue = 0;
  QString currency;
  for (auto &title : m_map.keys()) {
    auto purchasesList = m_map.value(title);
    numberOfItems = 0;
    int counter  = 1;
    for (auto &purchase : purchasesList) {
      numberOfItems = 0;
      int units = purchase.propertyByName(Property::UNITS).value().toInt();
      currency = purchase.propertyByName(Property::CUSTOMER_CURRENCY).stringValue();
      numberOfItems += units;
      totalItemsPerProduct += units;
      totalItems += units;
      float devProceeds = purchase.propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
      float devProceedsInEuro = purchase.propertyByName(Property::DEVELOPER_PROCEEDS_EUROS).value().toFloat();
      totalValuePerProduct += devProceedsInEuro;
      totalValue += devProceedsInEuro;
      appendLineToModel(QString::number(counter), title, numberOfItems, devProceeds, currency, devProceedsInEuro,
                        m_applePercentage, (m_applePercentage / 100) * devProceedsInEuro,
                        m_companyPercentage, (m_companyPercentage / 100) * devProceedsInEuro);
      counter++;
    }
    appendLineToModel("Subtotal", title, totalItemsPerProduct, 0,  "", totalValuePerProduct,
                      m_applePercentage, (m_applePercentage / 100) * totalValuePerProduct,
                      m_companyPercentage, (m_companyPercentage / 100) * totalValuePerProduct);
    totalItemsPerProduct = 0;
    totalValuePerProduct = 0;
  }
  appendLineToModel("Total", "", totalItems, 0, "", totalValue,
                    m_applePercentage, (m_applePercentage / 100) * totalValue,
                    m_companyPercentage, (m_companyPercentage / 100) * totalValue);
  return m_model;
}

void BalanceSheet::appendLineToModel(const QString &vHeader, const QString &title,
                                     const int &numberOfItems, const float &valueInCurrency,
                                     const QString &currency, const float &valueInBaseCurrency,
                                     double applePercentage, const float &appleRevenue,
                                     double companyPercentage, const float &companyRevenue) const {
  QStringList fields;
  fields << title << QString::number(numberOfItems) << QString::number((valueInCurrency))
         << currency << QString::number((valueInBaseCurrency))
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
