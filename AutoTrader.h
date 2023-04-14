#pragma once

#include "OrderBook.h"
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader();
        void callAutoTrader();
        void autoTradeMenu();
        int getUserOption(bool verbose=true);
        void processUserOption(int userOption);

    private:
        std::vector<std::string> availableCurrencies;
        OrderBook orderBook{"20200317.csv"}; // This should be moves, 
                                             // we load this in the MerkelMain
        int stopLossOption;
        int roi;
        std::map<std::string, double> currMap;

        void invalidChoice();
        void currencySelection();
        int stopLoss();
        int ROI();
        void autoStart();
        void autoStop();
        bool autoDetermineEnd();

};