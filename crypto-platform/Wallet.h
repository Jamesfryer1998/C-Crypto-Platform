#pragma once
#include <crypto-platform/OrderBookEntry.h>
#include <string>
#include <map>

class Wallet
{
    public:
        Wallet();
        bool insertCurrency(std::string type, double amount);
        bool removeCurrency(std::string, double amount);
        bool containsCurrency(std::string type, double amount);
        bool fulfillOrder(OrderBookEntry order);
        void processSale(OrderBookEntry sale);
        std::string toString();

    private:
        std::map<std::string, double> currencies;
};