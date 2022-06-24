#include "balancesheetrep.h"
#include <QDebug>

BalanceSheetRep::BalanceSheetRep()
{
}

BalanceSheetRep::~BalanceSheetRep()
{
}

void BalanceSheetRep::synthesizeFirstPartOfText()
{
    mHtmlText0 = "<html><head><title>HTML Online Editor Sample</title></head><body><h1>";
    mHtmlText0.append("<u><span style=\"font-size:16px;\">Balance Sheet Report</span></u></h1><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\">This report presents the purchases of every design in each currency that the design has been purchased. The purchases are being summed for every design and at the end of the report. The date format is dd/mm/yyyy.</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Author(s):</u> " + allAuthors + "</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Time Period:</u> from " + fromDate + ", to " + toDate + "</span></p><p>");
    mHtmlText0.append("<span style=\"font-size:12px;\"><u>Comments:</u> <span style=\"color:#ff0000;\">" + comments + "</span></p><hr />");
}

void BalanceSheetRep::synthesizeSecondPartOfText()
{
    mHtmlText1 = "<table align=\"left\" border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"width: 500px;\"><thead><tr>";
    mHtmlText1.append("<th width = 100 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Author</span></u></th>");
    mHtmlText1.append("<th width = 190 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Product</span></u></th>");
    mHtmlText1.append("<th width  = 50 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Items</span></u></th>");
    mHtmlText1.append("<th width  = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Revenue</span></u></th>");
    mHtmlText1.append("<th width  = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\">Currency</span></u></th>");
    mHtmlText1.append("<th width  = 70 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € Total </span></u></th>");
    mHtmlText1.append("<th width  = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % Apple </span></u></th>");
    mHtmlText1.append("<th width  = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> &#8364; Apple Rev. </span></u></th>");
    mHtmlText1.append("<th width  = 90 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % SoftwareHouse </span></u></th>");
    mHtmlText1.append("<th width  = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € SoftwareHouse Rev. </span></u></th>");
    mHtmlText1.append("<th width  = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> % Designer </span></u></th>");
    mHtmlText1.append("<th width  = 120 scope=\"col\" style=\"text-align: left; background-color: rgb(153, 153, 153);\"><u><span style=\"font-size:14px;\"> € Designer Rev. </span></u></th>");
    mHtmlText1.append("</tr></thead></table></body></html>");
    mHtmlText2 = "<html><head><title>HTML Online Editor Sample</title></head><body>";
    mHtmlText2 = "<table align=\"left\" border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"width: 500px;\">";
}

void BalanceSheetRep::appendLineinTable(QString name, QString parent, int numberOfItems, float moneyOfItems, QString currency, float eurosOfItems, float companyPercentage)
{
    mHtmlText2.append("<tr><td width  = 100><span style=\"font-size:12px;\">" + author + "</span></td>");
    mHtmlText2.append("<td width  = 190><span style=\"font-size:12px;\">" + name + "</span></td>");
    mHtmlText2.append("<td width  = 50><span style=\"font-size:12px;\">" + QString::number(numberOfItems) + "</span></td>");
    mHtmlText2.append("<td width  = 70><span style=\"font-size:12px;\">" + QString::number(moneyOfItems, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width  = 70><span style=\"font-size:12px;\">" + currency + "</span></td>");
    mHtmlText2.append("<td width  = 70><span style=\"font-size:12px;\">" + QString::number(eurosOfItems, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width  = 90><span style=\"font-size:12px;\"> 30% </span></td>");
    mHtmlText2.append("<td width  = 90><span style=\"font-size:12px;\">" + QString::number(eurosOfItems*0.3, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width  = 90><span style=\"font-size:12px;\">" + QString::number(companyPercentage * 100, 'f', 0) + "%</span></td>");
    mHtmlText2.append("<td width  = 120><span style=\"font-size:12px;\">" + QString::number(eurosOfItems * companyPercentage, 'f', 2) + "</span></td>");
    mHtmlText2.append("<td width  = 120><span style=\"font-size:12px;\">" + QString::number((0.7 - companyPercentage) * 100, 'f', 0) + "%</span></td>");
    mHtmlText2.append("<td width  = 120><span style=\"font-size:12px;\">" + QString::number(eurosOfItems * (0.7 - companyPercentage), 'f', 2) + "</span></td>");
    mHtmlText2.append("</tr>");
}

void BalanceSheetRep::appendTotalPerDesignInTable(QString name, QString parent, int numberOfItems, float eurosOfItemsPerDesign, float companyPercentage)
{
    QString str1 = "<td style=\"background-color: #CCFFFF; \"width = ";
    QString str2 = " <span style=\"text-align: left; font-size:12px;\"><strong>";
    mHtmlText2.append("<tr>" + str1 + "100" + str2 + author + "</strong></span></td>");
    mHtmlText2.append(str1 + "190" + str2 + name + "</strong></span></td>");
    mHtmlText2.append(str1 + "50" + str2 + QString::number(numberOfItems) + "</strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + "  </strong></span></td>");
    mHtmlText2.append(str1 + "70" + str2 + QString::number(eurosOfItemsPerDesign, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + " 30% </strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + QString::number(eurosOfItemsPerDesign*0.3, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "90" + str2 + QString::number(companyPercentage*100, 'f', 0) + "%</strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + QString::number(eurosOfItemsPerDesign * companyPercentage, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + QString::number((0.7 - companyPercentage)*100, 'f', 0) + "%</strong></span></td>");
    mHtmlText2.append(str1 + "120" + str2 + QString::number(eurosOfItemsPerDesign * (0.7 - companyPercentage), 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("</tr>");
}

void BalanceSheetRep::appendTotalOfReportInTable(int numberOfItems, float eurosOfItemsTotal, float companyRevenueTotal, float designerRevenueTotal)
{
    mHtmlText2.append("<tr><td width  = 100 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><u><strong>Total:</strong></u></span></td>");
    mHtmlText2.append("<td width  = 190 style=\"background-color: rgb(153, 153, 153);\">  </td>");
    mHtmlText2.append("<td width  = 50 style=\"background-color: rgb(153, 153, 153);\"><strong><span style=\"font-size:14px;\">" + QString::number(numberOfItems) + "</span></strong></td>");
    mHtmlText2.append("<td width  = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width  = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width  = 70 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(eurosOfItemsTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("<td width  = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong> 30% </strong></span></td>");
    mHtmlText2.append("<td width  = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(eurosOfItemsTotal*0.3, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("<td width  = 90 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width  = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(companyRevenueTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("<td width  = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>  </strong></span></td>");
    mHtmlText2.append("<td width  = 120 style=\"background-color: rgb(153, 153, 153);\"><span style=\"font-size:14px;\"><strong>" + QString::number(designerRevenueTotal, 'f', 2) + "</strong></span></td>");
    mHtmlText2.append("</tr>");
}

void BalanceSheetRep::appendFinalPartOfText()
{
    mHtmlText2.append("</table></body></html>");
}
