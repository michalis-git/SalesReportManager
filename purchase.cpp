#include "purchase.h"

#include <QString>

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
