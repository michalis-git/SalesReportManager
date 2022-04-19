#include "exchangerates.h"

#include <QDir>
#include <QFileInfoList>
#include <QTextStream>
#include <QDebug>


ExchangeRates::ExchangeRates() {
}

ExchangeRates *ExchangeRates::instance() {
  static ExchangeRates *rates = new ExchangeRates;
  return rates;
}

// line example: 2013-11-14 1.0553
QString dateFromLine(const QString &line) {
  return line.left(10);
}

float rateFromLine(const QString &line) {
  return line.rightRef(line.length() - 11).toFloat();
}

bool ExchangeRates::initialize(const QString &ratesPath, const QString &baseCurrency) {
  bool success;
  QString ratesPathToBase = ratesPath + QDir::separator() + baseCurrency;
  if (QDir(ratesPathToBase).exists()) {
    populateRatesMap(ratesPathToBase);
    success = true;
  } else
    success = false;

  return success;
}

bool ExchangeRates::changeBaseCurrency(const QString &ratesPath, const QString &baseCurrency) {
  bool success;
  QString ratesPathToBase = ratesPath + QDir::separator() + baseCurrency;
  if (QDir(ratesPathToBase).exists()) {
    m_ratesMap.clear();
    populateRatesMap(ratesPathToBase);
    success = true;
  } else
    success = false;

  return success;
}

// parses all txt files containing the exchange rates
// each file refers to a currency and contains rates for every date
// to a given base currency
void ExchangeRates::populateRatesMap(const QString &ratesPath) {
  QStringList nameFilter;
  nameFilter << "*.txt";
  QFileInfoList fileInfoList = QDir(ratesPath).entryInfoList(nameFilter, QDir::Files);

  for (auto &fileInfo : fileInfoList) {
    QMap <QString, float> datesRatesMap;
    QString currency = fileInfo.baseName().toUpper();
    QFile file(fileInfo.absoluteFilePath());
    if (file.open(QIODevice::ReadOnly)) {
      QTextStream textStream(&file);
      QString previousDate = "";
      float rateTotal = 0;
      int rateCounter = 0;
      while (true) {
        QString line = textStream.readLine();
        if (line.isNull())
          break;
        else {
          QString date = dateFromLine(line);
          float rate = rateFromLine(line);

          if (date == previousDate) {
            rateTotal += rate;
            rateCounter++;
            previousDate = date;
          } else {
            if(previousDate != "")
              datesRatesMap.insert(previousDate, rateTotal/rateCounter);
            datesRatesMap.insert(date, rate);
            previousDate = date;
            rateCounter = 1;
            rateTotal = rate;
          }
          datesRatesMap.insert(date, rateFromLine(line));
        }
      }
    }

    m_ratesMap.insert(currency, datesRatesMap);
  }
//    qDebug () << ratesPath << m_ratesMap;
}

ExchangeRatesMap *ExchangeRates::ratesMap() {
  return &m_ratesMap;
}

float ExchangeRates::rate(const QString &currency, const QString &date, RateErrorType &error) {
  error = NO_ERROR;
  float rate = -1;

  if (!m_ratesMap.contains(currency)) {
    error = RateErrorType::NO_DATA_FOR_CURRENCY;
    return rate;
  }
  if (!m_ratesMap.contains(currency)) {
    error = RateErrorType::NO_DATA_FOR_DATE;
    return rate;
  }

  return m_ratesMap.value(currency).value(date);
}
