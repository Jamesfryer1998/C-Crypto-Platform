#include <numeric>
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

double Stratergies::calcAveragePrice(std::vector<OrderBookEntry> orders)
{
    double sum = std::accumulate(orders.begin(), orders.end(), 0.0, [](double a, const OrderBookEntry& b) {
        return a + b.price;
    });
    return sum / orders.size();
}

// Mean reversion: This approach involves buying assets that have fallen in price and 
// selling assets that have risen in price. This strategy assumes that prices will 
// eventually return to their mean, or average, value.
std::string Stratergies::meanReversion(double current_price, double historical_average_price) {
    if (current_price > historical_average_price) {
        return "sell";
    } else if (current_price < historical_average_price) {
        return "buy";
    } else {
        return "hold";
    }
}


// Breakout trading: This approach involves identifying price levels at which a 
// cryptocurrency has historically struggled to move past, and trading in the direction
//  of the breakout when the price breaks through that level.

void Stratergies::breakout()
{

}


