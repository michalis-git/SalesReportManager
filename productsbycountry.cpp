#include "productsbycountry.h"

#include <QStandardItem>
#include <QDebug>


ProductsByCountry::ProductsByCountry(const Purchases &purchases)
    : FinancialReport(purchases) {
    m_title = QObject::tr("Products by Country Report");
    m_description = QObject::tr("This report presents the purchases of each product by country. ");
    m_description.append("The purchases are being summed by design. The date format is dd/mm/yyyy.");

    QList<Purchase> purchasesList = purchases.purchaseList();
    for (auto &purchase : purchasesList) {
        QString title = purchase.propertyByName(Property::TITLE).stringValue();
        QString country = purchase.propertyByName(Property::COUNTRY_CODE).stringValue();
        if (m_map.contains(title)) {
            CountryPurchasesMap countryPurchasesMap = m_map.value(title);
            if (countryPurchasesMap.contains(country)) {
                PurchasesList purchasesList = countryPurchasesMap.value(country);
                purchasesList << purchase;
                countryPurchasesMap.insert(country, purchasesList);
                m_map.insert(title, countryPurchasesMap);
            } else {
                countryPurchasesMap.insert(country, PurchasesList { purchase });
                m_map.insert(title, countryPurchasesMap);
            }
        } else {
            m_map.insert(title, CountryPurchasesMap{});
            CountryPurchasesMap countryPurchasesMap = m_map.value(title);
            countryPurchasesMap.insert(country, PurchasesList { purchase });
            m_map.insert(title, countryPurchasesMap);
        }
    }

    QStringList headers;
    headers << "Product" << "Country" << "Items"
            << "Total (€)" << "Apple %" << "Apple Revenue"
            << "SoftwareHouse %" << "SoftwareHouse Revenue";
    setHeadersToModel(headers);
}

QStandardItemModel *ProductsByCountry::getModel() const {
    int numberOfItems = 0, totalItemsPerProduct = 0, totalItems = 0;
    float valueOfItems = 0, totalValueperProduct = 0, totalValue = 0;
    for (auto title : m_map.keys()) {
        auto countryPurchasesMap = m_map.value(title);
        numberOfItems = 0;
        valueOfItems = 0;
        int counter  = 1;
        for (auto country : countryPurchasesMap.keys()) {
            auto purchasesList = countryPurchasesMap.value(country);
            numberOfItems = 0;
            valueOfItems = 0;
            for (auto purchase : purchasesList) {
                int units = purchase.propertyByName(Property::UNITS).value().toInt();
                numberOfItems += units;
                totalItemsPerProduct += units;
                totalItems += units;
                float devProceeds = purchase.propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
                valueOfItems  += devProceeds;
                totalValueperProduct += devProceeds;
                totalValue += devProceeds;
            }
            appendLineToModel(QString::number(counter), title, country, numberOfItems, valueOfItems);
            counter++;
        }
        appendLineToModel("Subtotal", title, "", totalItemsPerProduct, totalValueperProduct);
        totalItemsPerProduct = 0;
        totalValueperProduct = 0;
    }
    appendLineToModel("Total", "", "", totalItems, totalValue);
    return m_model;
}

void ProductsByCountry::appendLineToModel(const QString &vHeader, const QString &title, const QString &country,
                                          const int &numberOfItems, const float &valueOfItems) const {
    QStringList fields;
    fields << title << country << QString::number(numberOfItems) << QString::number((valueOfItems));
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
