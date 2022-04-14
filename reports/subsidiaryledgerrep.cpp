#include "subsidiaryledgerrep.h"

SubsidiaryLedgerRep::SubsidiaryLedgerRep()
{
}

SubsidiaryLedgerRep::~SubsidiaryLedgerRep()
{
}

void SubsidiaryLedgerRep::synthesizeFirstPartOfText()
{
    mHtmlText0 = "<html><head><title>HTML Online Editor Sample</title></head><body><h1>";
    mHtmlText0.append("<u><span style=\"font-size:16px;\">Subsidiary Ledger Report</span></u></h1><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\">This report presents all the purchases of the selected time period. The purchases are being sorted and summed by date. The date format is dd/mm/yyyy.</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Author(s):</u> " + allAuthors + "</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Time Period:</u> from " + fromDate + ", to " + toDate + "</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Comments:</u> <span style=\"color:#ff0000;\">" + comments + "</span></p><hr />");
}

void SubsidiaryLedgerRep::synthesizeSecondPartOfText()
{
    mHtmlText1.append("<table align=\"left\" border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"width: 500px;\"><thead><tr>");
    mHtmlText1.append("<th width = 100 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Date</span></u></th>");
    mHtmlText1.append("<th width = 100 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Author</span></u></th>");
    mHtmlText1.append("<th width = 180 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Product</span></u></th>");
    mHtmlText1.append("<th width = 50 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Items</span></u></th>");
    mHtmlText1.append("<th width = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Revenue</span></u></th>");
    mHtmlText1.append("<th width = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Currency</span></u></th>");
    mHtmlText1.append("<th width = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> &euro; Total </span></u></th>");
    mHtmlText1.append("<th width = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % Apple </span></u></th>");
    mHtmlText1.append("<th width = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € Apple Rev.</span></u></th>");
    mHtmlText1.append("<th width = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % Wings </span></u></th>");
    mHtmlText1.append("<th width = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € Wings Rev. </span></u></th>");
    mHtmlText1.append("<th width = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % Designer </span></u></th>");
    mHtmlText1.append("<th width = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € Designer Rev. </span></u></th>");
    mHtmlText1.append("</tr></thead></table></body></html>");
    mHtmlText2 = "<html><head><title>HTML Online Editor Sample</title></head><body>";
    mHtmlText2 = "<table align=\"left\" border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"width: 500px;\">";
}

void SubsidiaryLedgerRep::appendLineinTable(QString date, QString name, QString parentIdentifier, int numberOfItems, float moneyOfItems, QString currency, float eurosOfItems, float wingsPercentage)
{
    mHtmlText2.append("<tr><td width = 100 ><span style=\"font-size:12px;\">" + date + "</span></td>");
    mHtmlText2.append("<td width = 100 ><span style=\"font-size:12px;\">" + author + "</span></td>");
    mHtmlText2.append("<td width = 180><span style=\"font-size:12px;\">" + name + "</span></td>");
    mHtmlText2.append("<td width = 50><span style=\"font-size:12px;\">" + QString::number(numberOfItems) + "</span></td>");
    mHtmlText2.append("<td width = 70><span style=\"font-size:12px;\">" + QString::number(moneyOfItems, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width = 70><span style=\"font-size:12px;\">" + currency + "</span></td>");
    mHtmlText2.append("<td width = 70><span style=\"font-size:12px;\">" + QString::number(eurosOfItems, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width = 90><span style=\"font-size:12px;\"> 30% </span></td>");
    mHtmlText2.append("<td width = 90><span style=\"font-size:12px;\">" + QString::number(eurosOfItems*0.3, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width = 90><span style=\"font-size:12px;\">" + QString::number(wingsPercentage * 100) + "%</span></td>");
    mHtmlText2.append("<td width = 120><span style=\"font-size:12px;\">" + QString::number(eurosOfItems*wingsPercentage, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width = 120><span style=\"font-size:12px;\">" + QString::number((0.7-wingsPercentage) * 100, 'f', 0) + "%</span></td>");
    mHtmlText2.append("<td width = 120><span style=\"font-size:12px;\">" + QString::number(eurosOfItems*(0.7-wingsPercentage), 'f', 2) + "</span></td></tr>");
    mHtmlText2.append("</td>");
}

void SubsidiaryLedgerRep::appendTotalPerDateInTable(QString date, int numberOfItems, float eurosOfItems, float appleRevenue, float wingsRevenue, float designerRevenue)
{
    QString str1 = "<td style=\"background-color: #CCFFFF; \"width = ";
    QString str2 = " <span style=\"text-align: left; font-size:12px;\"><strong>";
    mHtmlText2.append("<tr>" + str1 + "100" + str2 + date + "</strong></span></td>");
    mHtmlText2.append(str1 + "100" + str2 + author + "</strong></span></td>");
    mHtmlText2.append(str1 + "180" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "50" + str2 + QString::number(numberOfItems) + "</strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + QString::number(eurosOfItems, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + "30%</strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + QString::number(appleRevenue, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + QString::number(wingsRevenue, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + QString::number(designerRevenue, 'f', 2) + "</strong></span></td></tr>");
    mHtmlText2.append("</td>");
}

void SubsidiaryLedgerRep::appendTotalOfReportInTable(int numberOfItems, float eurosOfItemsTotal, float appleTotal, float wingsTotal, float designerTotal)
{
    mHtmlText2.append("<tr><td width = 100 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><u><strong>Total:</strong></u></span></td>");
    mHtmlText2.append("<td width = 100 style=\"background-color: rgb(153, 153, 153);\">  </td>");
    mHtmlText2.append("<td width = 180 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\">  </span></td>");
    mHtmlText2.append("<td width = 50 style=\"background-color: rgb(153, 153, 153);\"><strong><span style=\"font-size:14px;\">" + QString::number(numberOfItems) + "</span></strong></td>");
    mHtmlText2.append("<td width = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(eurosOfItemsTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("<td width = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong> 30 % </strong></span></td>");
    mHtmlText2.append("<td width = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(appleTotal, 'f', 2) + "</strong></span></td>");
    if (author != "Wings Systems")
    {
        mHtmlText2.append("<td width = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    }
    else
    {
        mHtmlText2.append("<td width = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    }
    mHtmlText2.append("<td width = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(wingsTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("<td width = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(designerTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("</td>");
}

void SubsidiaryLedgerRep::appendFinalPartOfText()
{
    mHtmlText2.append("</table></body></html>");
}

