#include <cmath>
#include <iostream>
#include <crypto-platform/Stratergies.h>

Stratergies::Stratergies()
{

}

double Stratergies::fixedTradeSize(int numtrades, double currencyAmount)
{
    return currencyAmount/numtrades;
}

double Stratergies::percentTradeSize(int percent, double currencyAmount)
{
    return currencyAmount * percent / 100;
}

// double Stratergies::volatilityTradeSize(int percent, double currencyAmount)
// {
//     return currencyAmount * percent / 100;
// }


double Stratergies::volatilityTradeSize(const double* prices, int num_prices)
{
    // Calculate the standard deviation of prices
    double sum = 0;
    for (int i = 0; i < num_prices; i++) {
        sum += prices[i];
    }
    double mean = sum / num_prices;
    double variance = 0;
    for (int i = 0; i < num_prices; i++) {
        variance += pow(prices[i] - mean, 2);
    }
    double std_dev = sqrt(variance / num_prices);
    return std_dev;
}

double Stratergies::calculate_trade_size_multiplier(double volatility) 
{
    // Calculate the trade size multiplier based on volatility
    return 1 + (volatility / 100);
}

double Stratergies::adjust_trade_size(double base_trade_size, double trade_size_multiplier) 
{
    // Adjust the trade size based on the current volatility multiplier
    return base_trade_size * trade_size_multiplier;
}

void Stratergies::meanReversion()
{

}

void Stratergies::breakout()
{

}