#pragma once
#include <string>
#include <map>
#include <crypto-platform/OrderBookEntry.h>

class Stratergies
{
    public:
        Stratergies();

        // Trade Amount Methods
        double fixedTradeSize(int numtrades, double currencyAmount);
        double percentTradeSize(int percent, double currencyAmount);

        // double volatilityTradeSize(int percent, double currencyAmount);
        double volatilityTradeSize(const double* prices, int num_prices);
        double calculate_trade_size_multiplier(double volatility);
        double adjust_trade_size(double base_trade_size, double trade_size_multiplier);

        // Calculations
        double calcAveragePrice(std::vector<OrderBookEntry>& orders);
        double calcStandardDeviation(const std::vector<OrderBookEntry>& orders);

        // Stratergies 
        int meanReversion(double current_price, double historical_average_price);
        int boundReversion(double current_price, double historical_average_price, double stdev);
        void breakout();

    private:

};