#pragma once

#include <iostream>
#include <vector>
#include <numeric>

class TradeBookEntry{
 
    public:
        TradeBookEntry( std::string _timestamp,
                std::string _buyCurrency,
                std::string _sellCurrency,
                double _buyPrice,
                double _sellPrice);

        std::string timestamp;
        std::string buyCurrency;
        std::string sellCurrency;
        double buyPrice;
        double sellPrice;

};

