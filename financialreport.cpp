#include "financialreport.h"
#include "appsettings.h"

#include <QStandardItemModel>


FinancialReport::FinancialReport(const Purchases &purchases) {
  m_startDate = purchases.startDate();
  m_endDate = purchases.endDate();
  m_applePercentage = AppSettings::instance()->applePercentage();
  m_companyPercentage = 100 - m_applePercentage;

  m_model = new QStandardItemModel;
}

void FinancialReport::setHeadersToModel(const QStringList &headers) {
  int i = 0;
  for (auto &header : headers) {
    QStandardItem *headerItem = new QStandardItem(header);
    m_model->setHorizontalHeaderItem(i, headerItem);
    i++;
  }
}

void FinancialReport::styleItem(const QString &header, QStandardItem *item) const {
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
  }
}

const QString &FinancialReport::title() const {
  return m_title;
}

const QString &FinancialReport::description() const {
  return m_description;
}

const QDate &FinancialReport::startDate() const {
  return m_startDate;
}

const QDate &FinancialReport::endDate() const {
  return m_endDate;
}
