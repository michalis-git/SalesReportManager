#ifndef BYCOUNTRYREP_H
#define BYCOUNTRYREP_H

#include <QString>

class ByCountryRep
{
public:
    ByCountryRep();
    ~ByCountryRep();
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
    void appendLineinTable(QString name, QString country, int numberOfItems, float eurosOfItems, float companyPercentage);
    void appendTotalPerDesignInTable(QString name, int numberOfItems, float eurosOfItems, float companyPercentage);
    void appendTotalOfReportInTable(int numberOfItems, float eurosOfItemsTotal, float companyRevenueTotal, float designerRevenueTotal);
    void appendFinalPartOfText();

private:
    QString formatName(QString name);
};

#endif // BYCOUNTRYREP_H
