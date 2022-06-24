#ifndef BALANCESHEETREP_H
#define BALANCESHEETREP_H

#include <QString>

class BalanceSheetRep
{
public:
    BalanceSheetRep();
    ~BalanceSheetRep();
    QString mHtmlText0, mHtmlText1, mHtmlText2;
    QString author;
    QString fromDate;
    QString toDate;
    QString comments;
    QString allAuthors;

    void synthesizeText();
    void synthesizeFirstPartOfText();
    void synthesizeSecondPartOfText();
    void appendLineinTable(QString name, QString parent, int numberOfItems, float moneyOfItems, QString currency, float eurosOfItems, float companyPercentage);
    void appendTotalPerDesignInTable(QString name, QString parent, int numberOfItems, float eurosOfItemsPerDesign, float companyPercentage);
    void appendTotalOfReportInTable(int numberOfItems, float eurosOfItemsTotal, float companyRevenueTotal, float designerRevenueTotal);
    void appendFinalPartOfText();

private:
    QString formatName(QString name);
};

#endif // BALANCESHEETREP_H
