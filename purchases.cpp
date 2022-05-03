#include "appsettings.h"
#include "decompresser.h"
#include "purchases.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardItemModel>
#include <QFileInfoList>
#include <QDir>


QString Purchases::decompressFile(const QDate &date) {
    // Gets the file name from the selected date and creates the full path of the file.
    QString dateStr = date.toString("yyyyMMdd");
    QStringList nameFilter;
    QStringList list;
    nameFilter << "*" + dateStr+ ".txt.gz";
    QString reportsDirPath = AppSettings::instance()->reportsDirPath();
    QDir dir(reportsDirPath);

    QFileInfoList fileInfoList = dir.entryInfoList(nameFilter, QDir::Files);
    QString compressedFilePath;
    if (fileInfoList.count())
        compressedFilePath = fileInfoList.first().absoluteFilePath();
    else
        qDebug() << "Error! No report file for date" << date;

    QString decompressedDirPath = QDir::home().path() + "/tmpApplReports";
    Decompresser decompress(compressedFilePath, decompressedDirPath);
    return decompress.decompressedFilePath();
}

Purchases::Purchases(const QDate &fromDate, const QDate &toDate) {
    m_purchasesModel = new QStandardItemModel;
    QDate date = fromDate;
    while(date <= toDate) {
        QString decompressedPath = decompressFile(date);
        QFile file(decompressedPath);

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
                QStringList values = fileLineList.at(i).split("\t");
                Purchase *purchase = new Purchase(date, values);
                m_purchaseList << *purchase;
            }

            file.close();

        }
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
//            qDebug() << i << j << props[j].stringValue();
            m_purchasesModel->setItem(i, j, item);
        }
        i++;
    }
    return m_purchasesModel;
}

const QList<Purchase> &Purchases::purchaseList() const {
    return m_purchaseList;
}
