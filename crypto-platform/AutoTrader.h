#pragma once

#include <crypto-platform/OrderBook.h>
#include <crypto-platform/Wallet.h>
#include <crypto-platform/MatchingSystem.h>
#include <crypto-platform/Strategies.h>
#include <vector>
#include <string>
#include <map>

class AutoTrader{

    public:
        AutoTrader();
        void autoTradeMenu();
        void processUserOption(int userOption);
        void callAutoTrader(Wallet walletInput);

        // auto getCurrMap()
        // {
        //     return currMap;
        // }

    private:
        // Initisalise
        Wallet wallet;
        Wallet AutoWallet;
        MatchSystem match;
        Strategies strat;

        // Maps
        std::map<std::string, double> lossMap; // find a neater way to organise everything
        std::map<std::string, double> roiMap;

        // Variables
        int roi;
        int stopLossOption;

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