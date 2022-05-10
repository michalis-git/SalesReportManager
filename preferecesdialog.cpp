#include "preferecesdialog.h"
#include "appsettings.h"
#include "ui_preferecesdialog.h"

#include <QFileDialog>

PreferecesDialog::PreferecesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferecesDialog) {
    ui->setupUi(this);

    AppSettings *appSettings = AppSettings::instance();
    ui->reportsLineEdit->setText(appSettings->reportsDirPath());
    ui->ratesLineEdit->setText(appSettings->ratesDirPath());
    ui->applePercentageSpinBox->setValue(appSettings->applePercentage());

    connect(ui->reportsBrowseBtn, &QPushButton::clicked, this, &PreferecesDialog::onBrowseReportsClicked);
    connect(ui->ratesBrowseBtn, &QPushButton::clicked, this, &PreferecesDialog::onBrowseRatesClicked);
    connect(ui->applePercentageSpinBox, &QDoubleSpinBox::valueChanged, this, &PreferecesDialog::onAppleValueChanged);
}

PreferecesDialog::~PreferecesDialog() {
    delete ui;
}

void PreferecesDialog::onBrowseReportsClicked() {
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

void PreferecesDialog::onBrowseRatesClicked() {
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

void PreferecesDialog::onAppleValueChanged(double percentage) {
    AppSettings::instance()->changeApplePercentage(percentage);
}
