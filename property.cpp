#include "property.h"


Property::PropertyName Property::propertyName() const {
    return m_propertyName;
}

const QString &Property::stringValue() const {
    return m_stringValue;
}

const QString &Property::name() const {
    return m_name;
}

Property::DataType Property::dataType() const {
    return m_dataType;
}

Property::DataType Property::dataTypeFromPropertyName(PropertyName name) {
    switch (name) {
    case PROVIDER:
    case PROVIDER_COUNTRY:
    case SKU:
    case DEVELOPER:
    case TITLE:
    case VERSION:
    case PRODUCT_TYPE_ID:     // "Product Type Identifier"
    case UNITS:
    case DEVELOPER_PROCEEDS:
    case BEGIN_DATE:
    case END_DATE:
    case CUSTOMER_CURRENCY:
    case COUNTRY_CODE:
    case CURRENCY_OF_PROCEEDS:
    case APPLE_IDENTIFIER:
    case CUSTOMER_PRICE:
    case PROMO_CODE:
    case PARENT_IDENTIFIER:
    case SUBSCRIPTION:
    case PERIOD:
    case CATEGORY:
    case CMB:
        return STRING;
    default:
        return STRING;
    }
}

QString Property::nameFromPropertyName(PropertyName name) {
    switch (name) {
    case PROVIDER:
        return "Provider";
    case PROVIDER_COUNTRY:
        return "Provider country";
    case SKU:
        return "SKU";
    case DEVELOPER:
        return "Developer";
    case TITLE:
        return "Title";
    case VERSION:
        return "Version";
    case PRODUCT_TYPE_ID:     // "Product Type Identifier"
        return "Product type ID";
    case UNITS:
        return "Units";
    case DEVELOPER_PROCEEDS:
        return "Developer proceeds";
    case BEGIN_DATE:
        return "Begin date";
    case END_DATE:
        return "End date";
    case CUSTOMER_CURRENCY:
        return "Customer currency";
    case COUNTRY_CODE:
        return "Country code";
    case CURRENCY_OF_PROCEEDS:
        return "Currency of proceeds";
    case APPLE_IDENTIFIER:
        return "Apple ID";
    case CUSTOMER_PRICE:
        return "Customer price";
    case PROMO_CODE:
        return "Promo code";
    case PARENT_IDENTIFIER:
        return "Parent ID";
    case SUBSCRIPTION:
        return "Subscription";
    case PERIOD:
        return "Period";
    case CATEGORY:
        return "Category";
    case CMB:
        return "CMB";
    default:
        return "Unknown field";
    }
}

Property::Property(const PropertyName &propertyName, const QString &stringValue)
    : m_propertyName(propertyName), m_stringValue(stringValue) {
    m_name = nameFromPropertyName(propertyName);
    m_dataType = dataTypeFromPropertyName(propertyName);
}
