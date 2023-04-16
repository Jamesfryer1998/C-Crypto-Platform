#pragma once

#include <crypto-platform/OrderBook.h>
#include <crypto-platform/Wallet.h>
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader();
        void callAutoTrader();
        void autoTradeMenu();
        void processUserOption(int userOption);
 

    private:
        OrderBook orderBook{"20200317.csv"}; // This should be moves, 
                                             // we load this in the MerkelMain

        Wallet wallet;
        std::map<std::string, double> currMap;

        int stopLossOption;
        int roi;
        bool isAutoTraderRunning = false;

        int getUserOption(bool verbose=true);

        void invalidChoice();
        void currencySelectionNew();
        void currencySelection();
        int stopLoss();
        int ROI();
        void autoStart();
        void autoStop();
        bool determineEnd();
};