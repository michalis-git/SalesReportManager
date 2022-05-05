#include "productsbycountry.h"


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

void ProductsByCountry::appendLineToModel(const QString &title, const QString &country,
                                          const int &numberOfItems, const float &valueOfItems) {
    \QStandardItem *item = new QStandardItem()
}
