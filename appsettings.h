#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>

class QSettings;
class AppSettings
{
    AppSettings();
    AppSettings(AppSettings &other);
    void operator=(const AppSettings &);
    ~AppSettings();

    QSettings *m_settings;
    QString m_ratesDirPath;
    QString m_reportsDirPath;
    double m_applePercentage;
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
    double applePercentage() const;
    void changeApplePercentage(const double &percentage);
    void changeReportsPath(const QString &path);
    const QString exportLocation();
    void setExportLocation(const QString &exportLocation);
};

#endif // APPSETTINGS_H
