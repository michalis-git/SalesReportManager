#ifndef PREFERECESDIALOG_H
#define PREFERECESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferecesDialog;
}

class PreferecesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferecesDialog(QWidget *parent = nullptr);
    ~PreferecesDialog();

private:
    Ui::PreferecesDialog *ui;

private slots:
    void onBrowseReportsClicked();
    void onBrowseRatesClicked();
    void onAppleValueChanged(double percentage);
};

#endif // PREFERECESDIALOG_H
