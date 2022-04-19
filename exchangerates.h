#ifndef EXCHANGERATES_H
#define EXCHANGERATES_H

#include <QMap>
#include <QString>

typedef  QMap <QString, float> DatesRatesMap;
typedef  QMap <QString, QMap <QString, float>> ExchangeRatesMap;


class ExchangeRates
{
  std::string value_;
  ExchangeRates();
  ExchangeRates(ExchangeRates &other);
  void operator=(const ExchangeRates &);

  ExchangeRatesMap m_ratesMap;
public:
  enum  RateErrorType {
    NO_ERROR,
    NO_DATA_FOR_CURRENCY,
    NO_DATA_FOR_DATE
  };
  static ExchangeRates *instance();
  bool initialize(const QString &ratesPath, const QString& baseCurrency);
  bool changeBaseCurrency(const QString &ratesPath, const QString &baseCurrency);
  void populateRatesMap(const QString &ratesPath);
  ExchangeRatesMap *ratesMap();
  float rate(const QString &currency, const QString &date, RateErrorType &error);
};

#endif // EXCHANGERATES_H
