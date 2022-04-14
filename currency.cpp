#include "currency.h"
#include <qdebug.h>

Currency::Currency()
{
    numberOfDesigns = 0;
    amountOfMoney = 0;
}

Currency::~Currency()
{
}

void Currency::appendToMoney(float money)
{
    amountOfMoney += money;
//    qDebug() << "amountOfMoney" << amountOfMoney;
}

void Currency::appendToDesigns(int number)
{
    numberOfDesigns += number;
//    qDebug() << "numberOfDesigns" << numberOfDesigns;
}
