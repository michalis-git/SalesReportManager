#ifndef PROPERTY_H
#define PROPERTY_H

#include <QString>

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
  enum DataType { STRING, FLOAT };

      enum Name {
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
      };


private:
  QString m_title;
  QString m_stringValue;
  DataType m_dataType;

public:
  Property(const QString &title, const QString &stringValue, const DataType &dataType)
      : m_title(title), m_stringValue(stringValue), m_dataType(dataType) {};
};

#endif // PROPERTY_H
