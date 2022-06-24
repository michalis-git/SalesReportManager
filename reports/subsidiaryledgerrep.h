#ifndef SUBSIDIARYLEDGERREP_H
#define SUBSIDIARYLEDGERREP_H

#include <QString>

class SubsidiaryLedgerRep
{
public:
    SubsidiaryLedgerRep();
    ~SubsidiaryLedgerRep();
    QString mHtmlText0;
    QString mHtmlText1;
    QString mHtmlText2;
    QString author;
    QString allAuthors;
    QString fromDate;
    QString toDate;
    QString comments;

    void synthesizeText();
    void synthesizeFirstPartOfText();
    void synthesizeSecondPartOfText();
    void appendLineinTable(QString date, QString name, QString parentIdentifier, int numberOfItems, float moneyOfItems, QString currency, float eurosOfItems, float companyPercentage);
    void appendTotalPerDateInTable(QString date, int numberOfItems, float eurosOfItemsPerDesign, float appleRevenue, float companyRevenue, float designerRevenue);
    void appendTotalOfReportInTable(int numberOfItems, float eurosOfItemsTotal, float appleTotal, float companyTotal, float designerTotal);
    void appendFinalPartOfText();

private:
    QString formatName(QString name);
};

#endif // SUBSIDIARYLEDGERREP_H
