#include "purchase.h"
#include "exchangerates.h"
#include <QString>
#include <QDebug>

const QDate &Purchase::date() const {
    return m_date;
}

Purchase::Purchase(const QDate &date, const QStringList &valueList) {
    m_date = date;
    for (int i = 0; i < valueList.count(); i++) {
        QString value = valueList[i];
        Property::PropertyName propName = Property::PropertyName(i);
        m_properties << Property(propName, value);
    }
}

const QList<Property> &Purchase::properties() const {
    return m_properties;
}

Property Purchase::propertyByName(Property::PropertyName propertyName) {
    for (auto &property : m_properties) {
        if (property.propertyName() == propertyName) {
            return property;
            break;
        }
    }
    return Property{};
}

float Purchase::developerProceedsInEuros() {
    float devProceeds = propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
    QString customerCurrency = propertyByName(Property::CUSTOMER_CURRENCY).stringValue();

    ExchangeRates::RateErrorType error;
    float rate = ExchangeRates::instance()->rate(customerCurrency,
                                                 m_date.toString("yyyy-MM-dd"),
                                                 error);
    switch (error) {
    case ExchangeRates::NO_DATA_FOR_CURRENCY:
        qDebug() << "Error! no rates for " << customerCurrency << "!";
        break;
    case ExchangeRates::NO_DATA_FOR_DATE:
        qDebug() << "Error! No rate for " << customerCurrency
                 << " on " << m_date.toString("yyyy-MM-dd") << "!";
        break;
    default:
        break;
    }
    m_properties << Property(Property::DEVELOPER_PROCEEDS_EUROS, QString::number(rate * devProceeds));
}
