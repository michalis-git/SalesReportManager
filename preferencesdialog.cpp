#include "preferencesdialog.h"
#include "appsettings.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog) {
    ui->setupUi(this);

    AppSettings *appSettings = AppSettings::instance();
    ui->reportsLineEdit->setText(appSettings->reportsDirPath());
    ui->ratesLineEdit->setText(appSettings->ratesDirPath());
    ui->applePercentageSpinBox->setValue(appSettings->applePercentage());

    connect(ui->reportsBrowseBtn, &QPushButton::clicked, this, &PreferencesDialog::onBrowseReportsClicked);
    connect(ui->ratesBrowseBtn, &QPushButton::clicked, this, &PreferencesDialog::onBrowseRatesClicked);
//    connect(ui->applePercentageSpinBox, &QDoubleSpinBox::valueChanged, this, &PreferencesDialog::onAppleValueChanged);
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::onBrowseReportsClicked() {
    QString path = AppSettings::instance()->reportsDirPath();
    if (!QDir(path).exists())
        path = QDir::homePath();

    QFileDialog fileDialog(this, "Select daily reports directory", path);

    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);
    fileDialog.setWindowModality(Qt::ApplicationModal);
    int result = fileDialog.exec();
    if (result == 1) {
        path = fileDialog.selectedFiles().first();
        ui->reportsLineEdit->setText(path);
    }
}

void PreferencesDialog::onBrowseRatesClicked() {
    QString path = AppSettings::instance()->ratesDirPath();
    if (!QDir(path).exists())
        path = QDir::homePath();

    QFileDialog fileDialog(this, "Select daily reports directory", path);

    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly);
    fileDialog.setWindowModality(Qt::ApplicationModal);
    int result = fileDialog.exec();
    if (result == 1) {
        path = fileDialog.selectedFiles().first();
        ui->ratesLineEdit->setText(path);
    }
}

void PreferencesDialog::onAppleValueChanged(double percentage) {
    AppSettings::instance()->changeApplePercentage(percentage);
}
