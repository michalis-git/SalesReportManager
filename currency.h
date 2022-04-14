#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>

class Currency
{
public:
    Currency();
    ~Currency();
    QString currencyName;
    int numberOfDesigns;
    float amountOfMoney;

    void appendToMoney(float money);
    void appendToDesigns(int number);
};

#endif // CURRENCY_H
