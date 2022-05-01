#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>

class AppSettings
{
    AppSettings();
    AppSettings(AppSettings &other);
    void operator=(const AppSettings &);

    QString m_ratesDirPath;
    QString m_reportsDirPath;
public:
    enum  ErrorType {
      NO_ERROR,
      RATES_DIR_NOT_EXIST,
      REPORTS_DIR_NOT_EXIST,
      NO_RATES_FOR_BASE_CURRENCY,
    };
    static AppSettings *instance();
    bool initialize(int &error);
    const QString &ratesDirPath() const;
    const QString &reportsDirPath() const;
};

#endif // APPSETTINGS_H
