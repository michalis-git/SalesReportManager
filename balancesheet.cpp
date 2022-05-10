#include "balancesheet.h"

#include <QStandardItem>
#include <QDebug>


BalanceSheet::BalanceSheet(const Purchases &purchases) {
  m_title = QObject::tr("Subsidiary Ledger Report");
  m_description = QObject::tr("This report presents all the purchases of the selected time period.");
  m_description.append("The purchases are being sorted and summed by date. The date format is dd/mm/yyyy.");
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
      appendLineToModel(QString::number(counter), title, numberOfItems, devProceeds, currency, devProceedsInEuro);
      counter++;
    }
    appendLineToModel("Subtotal", title, totalItemsPerProduct, 0,  "", totalValuePerProduct);
    totalItemsPerProduct = 0;
    totalValuePerProduct = 0;
  }
  appendLineToModel("Total", "", totalItems, 0, "", totalValue);
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
          << "Apple Revenue %" << "Apple Revenue %"
          << "SoftwareHouse %" << "SoftwareHouse Revenue";

  int i = 0;
  for (auto header : headers) {
    QStandardItem *headerItem = new QStandardItem(header);
    m_model->setHorizontalHeaderItem(i, headerItem);
    i++;
  }

}

void BalanceSheet::styleItem(int row, const QString &header, QStandardItem *item) const {
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
                                     const QString &currency, const float &valueInBaseCurrency) const {
  QStringList fields;
  fields << title << QString::number(numberOfItems) << QString::number((valueInCurrency))
         << currency << QString::number((valueInBaseCurrency));
  int j = 0;
  int row = m_model->rowCount();
  for (auto &field : fields) {
    QStandardItem *item = new QStandardItem(field);
    styleItem(row, vHeader, item);
    m_model->setItem(row, j, item);
    j++;
  }

  QStandardItem *headerItem = new QStandardItem(vHeader);
  //    headerItem->setBackground(QBrush("#daedf4"));
  styleItem(row, vHeader, headerItem);
  m_model->setVerticalHeaderItem(row, headerItem);

}
