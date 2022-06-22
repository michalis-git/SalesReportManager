#include "appsettings.h"
#include "exchangerates.h"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>


#define EXCHANGE_RATES_DIR_PATH "/exchange_rates";
#define DAILY_REPORTS_DIR_PATH "/apple_daily_reports";


AppSettings::AppSettings() {
}

AppSettings::~AppSettings() {
    delete m_settings;
}


AppSettings *AppSettings::instance() {
    static AppSettings *appSettings = new AppSettings;
    return appSettings;
}

bool AppSettings::initialize(int &error) {
    error = NO_ERROR;
    m_ratesDirPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            + EXCHANGE_RATES_DIR_PATH;

    if (!QDir(m_ratesDirPath).exists()) {
        error = RATES_DIR_NOT_EXIST;
    } else {
        ExchangeRates *rates = ExchangeRates::instance();
        if (!rates->initialize(m_ratesDirPath, "toEUR"))
            error = NO_RATES_FOR_BASE_CURRENCY;
    }

    m_settings = new QSettings("SMT");
    QString defDailyReportsPath = QDir::homePath() + DAILY_REPORTS_DIR_PATH;
    m_reportsDirPath = m_settings->value("dailyReportsDirPath", defDailyReportsPath).toString();
    if (!QDir(m_reportsDirPath).exists())
        error = REPORTS_DIR_NOT_EXIST;

    m_applePercentage = m_settings->value("applePercentage", 30).toDouble();

}

const QString &AppSettings::reportsDirPath() const {
    return m_reportsDirPath;
}

const QString &AppSettings::ratesDirPath() const {
    return m_ratesDirPath;
}

const QString AppSettings::exportLocation() {
    QString defExportLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString exportLocation = m_settings->value("exportLocation", defExportLocation).toString();
    return exportLocation;
}

void AppSettings::setExportLocation(const QString &exportLocation) {
    m_settings->setValue("exportLocation", exportLocation);
}

double AppSettings::applePercentage() const {
    return m_applePercentage;
}

void AppSettings::changeApplePercentage(const double &percentage) {
    QSettings settings("SMT");
    settings.setValue("applePercentage", percentage);
}
