#include <crypto-platform/Wallet.h>
#include <crypto-platform/CSVReader.h>
#include <iostream>

Wallet::Wallet()
{

}

bool Wallet::insertCurrency(std::string type, double amount)
{
    try{
        double balance;
        if (amount < 0)
        {
            return false;
        }
        if (currencies.count(type) == 0)
        {
            balance = 0;
        }
        else
        {
            balance = currencies[type];
        }
        balance += amount;
        currencies[type] = balance;
        return true;
    }catch(const std::exception& e)
    {
        return false;
    }
}

bool Wallet::removeCurrency(std::string type, double amount)
{
    try{
        if (amount < 0)
        {
            return false;
        }
        if (currencies.count(type) == 0)
        {
            return false;
        }
        else
        {
            if (containsCurrency(type, amount))
            {
                currencies[type] -= amount;
                return true;
            }
            else
                return false;
        }
    }catch(const std::exception& e)
    {
        return false;
    }
}

bool Wallet::containsCurrency(std::string type, double amount)
{
    if (currencies.count(type) == 0)
        return false;
    else 
        return currencies[type] >= amount;
}

std::string Wallet::toString()
{   
    std::string currencyString;
    for (std::pair<std::string, double> pair: currencies)
    {
        std::string currencyName = pair.first;
        double currencyAmount = pair.second;
        currencyString += currencyName + " : " + std::to_string(currencyAmount) + "\n";
    }
    return currencyString;
}

bool Wallet::fulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    if (order.type == OrderBookType::ask)
    {
        double amount = order.amount;
        std::string currency = currs[0];
        std::cout << "Wallet::fulfillOrder " << currency << " : " << amount << std::endl; 
        return containsCurrency(currency, amount);
    }
    if (order.type == OrderBookType::bid)
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        std::cout << "Wallet::fulfillOrder " << currency << " : " << amount << std::endl; 
        return containsCurrency(currency, amount);
    }
    return false;
}

void Wallet::processSale(OrderBookEntry sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    if (sale.type == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
    }
    if (sale.type == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
    }
}