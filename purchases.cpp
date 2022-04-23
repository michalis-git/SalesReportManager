#include "purchases.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardItemModel>


Purchases::Purchases(const QDate &date, const QString &path) {
    m_purchasesModel = new QStandardItemModel;
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        QStringList fileLineList;
        QTextStream textStream(&file);
        while (true) {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                fileLineList.append(line);
        }


        for (int i = 1; i < fileLineList.count(); i++) {
            qDebug() << fileLineList[i];
            QStringList values = fileLineList.at(i).split("\t");
            Purchase *purchase = new Purchase(date, values);
            m_purchaseList << *purchase;
        }

        file.close();

    }
}

Purchases::Purchases(const QDate &fromDate, const QDate &toDate, const QString &path) {
    QDate date = fromDate;
    while (date < toDate) {


        date = date.addDays(1);
    }
}

QStandardItemModel *Purchases::purchasesModel() const {
//    qDebug() << m_purchaseMap.values().count();
    for (int i = Property::PROVIDER; i != Property::CMB; i++) {
        Property::PropertyName propName = static_cast<Property::PropertyName>(i);
        QStandardItem *headerItem = new QStandardItem(Property::nameFromPropertyName(propName));
        m_purchasesModel->setHorizontalHeaderItem(i, headerItem);
    }

    int i = 0;
    for (auto &purchase : m_purchaseList) {
        QList<Property> props = purchase.properties();
        for (int j = 0; j < props.count(); j++) {
            QStandardItem *item = new QStandardItem(props[j].stringValue());
            qDebug() << i << j << props[j].stringValue();
            m_purchasesModel->setItem(i, j, item);
        }
        i++;
    }
    return m_purchasesModel;
}
