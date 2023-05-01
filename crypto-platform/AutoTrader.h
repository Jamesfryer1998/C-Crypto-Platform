#pragma once

#include <crypto-platform/OrderBook.h>
#include <crypto-platform/Wallet.h>
#include <crypto-platform/MatchingSystem.h>
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader();
        void autoTradeMenu();
        void processUserOption(int userOption);
        void callAutoTrader(Wallet walletInput);

        auto getCurrMap()
        {
            return currMap;
        }

    private:
        Wallet wallet;
        MatchSystem match;
        
        // Maps
        std::map<std::string, double> currMap;
        std::map<std::string, double> tradeCurrMap; // Probs need to change these variables,
        std::map<std::string, double> lossMap; // find a neater way to organise everything
        std::map<std::string, double> roiMap;

        // Variables
        int roi;
        int stopLossOption;
        // int tradeCount = 0;// Hard coded for now, allow a user input
        // int numberTrades = 1000; // Used to define the amount the trade
        bool isAutoTraderRunning = false;

        // User input
        void ROI();
        void stopLoss();
        void autoStop();
        void autoStart();
        bool determineEnd();
        void invalidChoice();
        void currencySelection();
        int getUserOption(bool verbose=true);

        void generateTrades();
};