#pragma once
#include <vector>
#include "crypto-platform/OrderBookEntry.h"
#include "crypto-platform/OrderBook.h"
#include "crypto-platform/Wallet.h"
#include "crypto-platform/AutoTrader.h"

class MerkelMain
{
    public:
        MerkelMain();
        void init();

    private:
        void printMenu();
        void printHelp();
        void printOrderBookStats();
        void exchangeStats();
        void makeAsk();
        void makeBid();
        void walletState();
        void processTrade();
        // void callAutoTrader();
        void invalidChoice();
        int getUserOption();
        void processUserOption(int userOption);
        int computeAveragePrice(std::vector<double> prices);
        int computeLowPrice(std::vector<double> prices);
        int computeHighPrice(std::vector<double> prices);
        int computePriceSpread(std::vector<double> prices);

        std::string currentTime;

        OrderBook orderBook{"20200317.csv"};

        Wallet wallet;

        AutoTrader autoTrade;
};