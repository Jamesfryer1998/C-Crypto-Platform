#pragma once

#include <crypto-platform/OrderBook.h>
#include <crypto-platform/Wallet.h>
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader(OrderBook _orderbook);
        OrderBook orderBook;
        void autoTradeMenu();
        void processUserOption(int userOption);
        void callAutoTrader(Wallet walletInput);


    private:
        Wallet wallet;
        // OrderBook orderBook{"20200317.csv"}; // This should be moved, we load this in the MerkelMain

        std::map<std::string, double> currMap;
        std::map<std::string, double> tradeCurrMap; // Probs need to change these variables,
        std::map<std::string, double> lossMap; // find a neater way to organise everything
        std::map<std::string, double> roiMap;

        int numberTrades = 1000; 
        int tradeCount = 0;// Hard coded for now, allow a user input
        int roi;
        int stopLossOption;
        bool isAutoTraderRunning = false;

        void ROI();
        void stopLoss();
        void autoStop();
        void autoStart();
        bool determineEnd();
        void invalidChoice();
        void currencySelectionNew();
        int getUserOption(bool verbose=true);
        void autoAsk(std::string product, std::string timestamp);
        void autoBid(std::string product, std::string timestamp);
};