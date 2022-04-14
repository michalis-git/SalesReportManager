#include "dailyreport.h"
#include <QStringList>
#include <QDate>
#include <QDebug>

DailyReport::DailyReport()
{
    date = new QDate;
}

DailyReport::~DailyReport()
{
}

void DailyReport::setDate(QDate* myDate)
{
    date = myDate;
}

QDate* DailyReport::getDate()
{
    return date;
}

