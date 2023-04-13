#pragma once

#include <vector>
#include <string>

class AutoTrader{

    public:
        AutoTrader();
        static void callAutoTrader();
        static void autoTradeMenu();
        static int getUserOption();

    private:
        std::vector<std::string> availableCurrencies;

        void autoStart();
        void autoStop();
        bool autoDetermineEnd();

};