#pragma once

#include <crypto-platform/OrderBook.h>
#include <crypto-platform/Wallet.h>
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader();
        void autoTradeMenu();
        void processUserOption(int userOption);
        void callAutoTrader(Wallet walletInput);

    private:
        Wallet wallet;
        OrderBook orderBook{"20200317.csv"}; // This should be moved, we load this in the MerkelMain
        // FIXME: How can I get the order book from MerkelMain?? 
        // - If I load this in there I will have a plain OrderBook do I want that?

        std::map<std::string, double> currMap;

        int roi;
        int stopLossOption;
        bool isAutoTraderRunning = false;

        int ROI();
        int stopLoss();
        void autoStop();
        void autoStart();
        bool determineEnd();
        void invalidChoice();
        void currencySelection();
        void currencySelectionNew();
        int getUserOption(bool verbose=true);
};