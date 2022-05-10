#include "purchase.h"
#include "exchangerates.h"
#include "appsettings.h"
#include <QString>
#include <QDebug>
#include <QDir>

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
    setDeveloperProceedsInEuros();
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

void Purchase::setDeveloperProceedsInEuros() {
    float devProceeds = propertyByName(Property::DEVELOPER_PROCEEDS).value().toFloat();
    QString customerCurrency = propertyByName(Property::COUNTRY_CODE).stringValue();

    ExchangeRates::RateErrorType error;
    QString ratesPath = AppSettings::instance()->ratesDirPath() + QDir::separator() + "toEUR";
    ExchangeRates::instance()->initialize(ratesPath, "toEUR");
    float rate = ExchangeRates::instance()->rate(customerCurrency,
                                                 m_date.toString("yyyy-MM-dd"),
                                                 error);
//    qDebug() << rate << "**";
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
