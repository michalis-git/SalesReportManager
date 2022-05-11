#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>
#include <QVariant>

//   // "Provider"
//QString m_provider;
//// "Provider Country"
//QString m_providerCountry;
//// "SKU"
//QString m_sku;
//// "Developer"
//QString m_developer;
//// "Title"
//QString m_title;
//// "Version"
//QString m_version;
//// "Product Type Identifier"
//QString m_prodTypeIdentfier;
//// "Units"
//QString m_units;
//// "Developer Proceeds
//QString m_devProceeds;
//// Begin Date
//QDate m_beginDate;
//// End Date
//QDate m_endDate;
//// Customer Currency
//QString m_customerCurrency;
//// Country Code
//QString m_countryCode;
//// Currency of Proceeds
//QString m_currencyOfProceeds;
//// Apple Identifier
//QString m_appleIdentifier;
//// Customer Price
//float m_customerPrice = 0;
//// Promo Code
//QString m_promoCode;
//// Parent Identifier
//QString m_parentIdentifier;
//// Subscription
//QString m_subscription;
//// Period
//QString m_period;
//// Category
//QString m_category;
//// CMB"
//QString mCMB;



class Property
{
public:
    enum DataType { INT, FLOAT, DATE, STRING };

    enum PropertyName {
        PROVIDER,
        PROVIDER_COUNTRY,
        SKU,
        DEVELOPER,
        TITLE,
        VERSION,
        PRODUCT_TYPE_ID,     // "Product Type Identifier"
        UNITS,
        DEVELOPER_PROCEEDS,
        BEGIN_DATE,
        END_DATE,
        CUSTOMER_CURRENCY,
        COUNTRY_CODE,
        CURRENCY_OF_PROCEEDS,
        APPLE_IDENTIFIER,
        CUSTOMER_PRICE,
        PROMO_CODE,
        PARENT_IDENTIFIER,
        SUBSCRIPTION,
        PERIOD,
        CATEGORY,
        CMB,
        DEVELOPER_PROCEEDS_EUROS,

        //Product by country
        NUMBER_OF_ITEMS,
        VALUE_OF_ITEMS_IN_CURRENCY,
        COMPANY_REVENUE,
        APPLE_REVENUE,
    };


private:
    PropertyName m_propertyName;
    QString m_stringValue;
    QVariant m_value;
    QString m_name;
    DataType m_dataType;
    DataType dataTypeFromPropertyName(PropertyName name);

public:
    Property(const PropertyName &propertyName, const QString &stringValue);
    Property() = default;
    static QString nameFromPropertyName(PropertyName name);
    PropertyName propertyName() const;
    const QString &stringValue() const;
    const QString &name() const;
    Property::DataType dataType() const;
    const QVariant &value() const;
};

#endif // PROPERTY_H
