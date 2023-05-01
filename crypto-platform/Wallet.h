#pragma once
#include <crypto-platform/OrderBookEntry.h>
#include <string>
#include <map>

class Wallet
{
    public:
        Wallet();

        // Wallet Operations
        std::string toString();
        void processSale(OrderBookEntry sale);
        bool fulfillOrder(OrderBookEntry order);
        bool removeCurrency(std::string, double amount);
        bool insertCurrency(std::string type, double amount);
        bool containsCurrency(std::string type, double amount);

    private:
        // Variables
        std::map<std::string, double> currencies;
};