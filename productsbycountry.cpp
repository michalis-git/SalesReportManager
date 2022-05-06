#include "productsbycountry.h"

#include <QStandardItem>


ProductsByCountry::ProductsByCountry(const Purchases &purchases) {
    QList<Purchase> purchasesList = purchases.purchaseList();
    for (auto &purchase : purchasesList) {
        QString title = purchase.propertyByName(Property::TITLE).stringValue();
        QString country = purchase.propertyByName(Property::COUNTRY_CODE).stringValue();
        if (m_map.contains(title)) {
            CountryPurchasesMap countryPurchasesMap = m_map.value(title);
            if (countryPurchasesMap.contains(country)) {
                PurchasesList purchasesList = countryPurchasesMap.value(country);
                purchasesList << purchase;
            } else {
                countryPurchasesMap.insert(country, PurchasesList { purchase });
            }
        } else {
            m_map.insert(title, CountryPurchasesMap{});
            CountryPurchasesMap countryPurchasesMap = m_map.value(title);
            countryPurchasesMap.insert(country, PurchasesList { purchase });
        }
    }

    m_model = new QStandardItemModel;
    setHeadersToModel();
}

QStandardItemModel *ProductsByCountry::getModel() const {
    int numberOfItems = 0;
    float valueOfItems = 0;
    for (auto title : m_map.keys()) {
        auto countryPurchasesMap = m_map.value(title);
        numberOfItems = 0;
        valueOfItems = 0;
        for (auto country : countryPurchasesMap.keys()) {
            auto purchasesList = countryPurchasesMap.value(country);
            numberOfItems = 0;
            valueOfItems = 0;
            for (auto purchase : purchasesList) {
                numberOfItems += purchase.propertyByName(Property::UNITS).value().toInt();
                valueOfItems  += purchase.propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
            }
            appendLineToModel(title, country, numberOfItems, valueOfItems);
        }
        appendLineToModel(title, "", numberOfItems, valueOfItems);
    }
    appendLineToModel("", "", numberOfItems, valueOfItems);
    return m_model;
}

void ProductsByCountry::setHeadersToModel() {
  QStringList headers;
  headers << "Design" << "Country" << "Items"
          << "Total (€)" << "Apple %" << "Apple Revenue"
          << "SoftwareHouse %" << "SoftwareHouse Revenue";

  int i = 0;
  for (auto header : headers) {
    QStandardItem *headerItem = new QStandardItem(header);
    m_model->setHorizontalHeaderItem(i, headerItem);
    i++;
  }

}

void ProductsByCountry::appendLineToModel(const QString &title, const QString &country,
                                          const int &numberOfItems, const float &valueOfItems) const {
  QStringList fields;
  fields << title << country << QString::number(numberOfItems) << QString::number((valueOfItems));
  int j = 0;
  int row = m_model->rowCount();
  for (auto &field : fields) {
    QStandardItem *item = new QStandardItem(field);
    m_model->setItem(row, j, item);
    j++;
  }
}
