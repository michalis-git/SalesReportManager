#include "productsbycountry.h"

#include <QStandardItem>
#include <QDebug>


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

    m_model = new QStandardItemModel;
    setHeadersToModel();
}

QStandardItemModel *ProductsByCountry::getModel() const {
    int numberOfItems = 0, totalItemsPerProduct = 0, totalItems = 0;
    float valueOfItems = 0, totalValueperProduct = 0, totalValue = 0;
    for (auto title : m_map.keys()) {
        qDebug() << m_map.keys();
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

void ProductsByCountry::setHeadersToModel() {
    QStringList headers;
    headers << "Product" << "Country" << "Items"
            << "Total (€)" << "Apple %" << "Apple Revenue"
            << "SoftwareHouse %" << "SoftwareHouse Revenue";

    int i = 0;
    for (auto header : headers) {
        QStandardItem *headerItem = new QStandardItem(header);
        m_model->setHorizontalHeaderItem(i, headerItem);
        i++;
    }

}

void ProductsByCountry::styleItem(int row, const QString &header, QStandardItem *item) const {
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

void ProductsByCountry::appendLineToModel(const QString &vHeader, const QString &title, const QString &country,
                                          const int &numberOfItems, const float &valueOfItems) const {
    QStringList fields;
    fields << title << country << QString::number(numberOfItems) << QString::number((valueOfItems));
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
