#ifndef PURCHASE_H
#define PURCHASE_H

#include <QDate>
#include <QList>
#include "property.h"


class Purchase
{
    QList<Property> m_properties;
    QDate m_date;

public:
    Purchase(const QDate &date, const QStringList &valueList);
    const QList<Property> &properties() const;
    Property propertyByName(Property::PropertyName propertyName);
    void setDeveloperProceedsInEuros();
    const QDate &date() const;
};

#endif // PURCHASE_H
