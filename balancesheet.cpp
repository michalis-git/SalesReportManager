#include "appsettings.h"
#include "balancesheet.h"

#include <QStandardItem>
#include <QDebug>


BalanceSheet::BalanceSheet(const Purchases &purchases) {
  m_title = QObject::tr("Balance Sheet Report");
  m_description = QObject::tr("This report presents the purchases of every design in each currency that the design has been purchased. ");
  m_description.append("The purchases are being summed for every design and at the end of the report. The date format is dd/mm/yyyy.");
  m_startDate = purchases.startDate();
  m_endDate = purchases.endDate();
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

  m_model = new QStandardItemModel;
  setHeadersToModel();
}

QStandardItemModel *BalanceSheet::getModel() const {
  int numberOfItems = 0, totalItemsPerProduct = 0, totalItems = 0;
  float valueOfItems = 0, totalValuePerProduct = 0, totalValue = 0;
  QString currency;
  double applePercentage = AppSettings::instance()->applePercentage();
  double companyPercentage = 100 - applePercentage;
  for (auto title : m_map.keys()) {
    auto purchasesList = m_map.value(title);
    numberOfItems = 0;
    valueOfItems = 0;
    int counter  = 1;
    for (auto purchase : purchasesList) {
      numberOfItems = 0;
      valueOfItems = 0;
      int units = purchase.propertyByName(Property::UNITS).value().toInt();
      currency = purchase.propertyByName(Property::CUSTOMER_CURRENCY).stringValue();
      numberOfItems += units;
      totalItemsPerProduct += units;
      totalItems += units;
      float devProceeds = purchase.propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
      float devProceedsInEuro = purchase.propertyByName(Property::DEVELOPER_PROCEEDS_EUROS).value().toFloat();
      valueOfItems  += devProceeds;
      totalValuePerProduct += devProceedsInEuro;
      totalValue += devProceedsInEuro;
      appendLineToModel(QString::number(counter), title, numberOfItems, devProceeds, currency, devProceedsInEuro,
                        applePercentage, (applePercentage / 100) * devProceedsInEuro,
                        companyPercentage, (companyPercentage / 100) * devProceedsInEuro);
      counter++;
    }
    appendLineToModel("Subtotal", title, totalItemsPerProduct, 0,  "", totalValuePerProduct,
                      applePercentage, (applePercentage / 100) * totalValuePerProduct,
                      companyPercentage, (companyPercentage / 100) * totalValuePerProduct);
    totalItemsPerProduct = 0;
    totalValuePerProduct = 0;
  }
  appendLineToModel("Total", "", totalItems, 0, "", totalValue,
                    applePercentage, (applePercentage / 100) * totalValue,
                    companyPercentage, (companyPercentage / 100) * totalValue);
  return m_model;
}

const QString &BalanceSheet::title() const {
  return m_title;
}

const QString &BalanceSheet::description() const {
  return m_description;
}

const QDate &BalanceSheet::startDate() const {
  return m_startDate;
}

const QDate &BalanceSheet::endDate() const {
  return m_endDate;
}

void BalanceSheet::setHeadersToModel() {
  QStringList headers;
  headers << "Product" << "Items" << "Revenue"
          << "Currency" << "Revenue €"
          << "Apple %" << "Apple €"
          << "SoftwareHouse %" << "SoftwareHouse €";

  int i = 0;
  for (auto header : headers) {
    QStandardItem *headerItem = new QStandardItem(header);
    m_model->setHorizontalHeaderItem(i, headerItem);
    i++;
  }

}

void BalanceSheet::styleItem(const QString &header, QStandardItem *item) const {
  if (header == "Total") {
    QFont  font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setBackground(QBrush(QColor("#80c3d8")));
  }
  if (header == "Subtotal") {
    QFont  font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setBackground(QBrush(QColor("#daedf4")));
    //        m_model->setHeaderData(row, Qt::Orientation::Vertical,
    //                               QVariant(QBrush(QColor("#daedf4"))),
    //                               Qt::BackgroundColorRole);
  }
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
