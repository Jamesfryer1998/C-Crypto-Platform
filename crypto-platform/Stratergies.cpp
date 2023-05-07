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

double Stratergies::calcAveragePrice(std::vector<OrderBookEntry>& orders)
{
    double sum = std::accumulate(orders.begin(), orders.end(), 0.0, [](double a, const OrderBookEntry& b) {
        return a + b.price;
    });



    return sum / orders.size();
}

// double Stratergies::calcStandardDeviation(std::vector<OrderBookEntry>& orders)
// {

// }

double Stratergies::calcStandardDeviation(const std::vector<OrderBookEntry>& entries) 
{
    std::vector<double> prices(entries.size());
    std::transform(entries.begin(), entries.end(), prices.begin(),
                   [](const OrderBookEntry& entry) { return entry.price; });
    double sum = std::accumulate(prices.begin(), prices.end(), 0.0);
    double mean = sum / prices.size();
    double sq_sum = std::inner_product(prices.begin(), prices.end(), prices.begin(), 0.0);
    double stdev = std::sqrt(sq_sum / prices.size() - mean * mean);
    return round(stdev * 100.0) / 100.0;
}

/* Mean reversion staratergy
0 = sell
1 = buy
2 = hold*/
int Stratergies::meanReversion(double currentPrice, double histAveragePrice)
{
    if (currentPrice > histAveragePrice) 
    {
        return 0;
    } 
    else if (currentPrice < histAveragePrice) 
    {
        return 1;
    } 
    else {
        return 2;
    }
}

int Stratergies::boundReversion(double currentPrice, double histAveragePrice, double stdev) 
{
    double upper_band = histAveragePrice + stdev;
    double lower_band = histAveragePrice - stdev;
    
    if (currentPrice > upper_band) {
        return 0;
    } else if (currentPrice < lower_band) {
        return 1;
    } else {
        return 2;
    }

}






// Breakout trading: This approach involves identifying price levels at which a 
// cryptocurrency has historically struggled to move past, and trading in the direction
//  of the breakout when the price breaks through that level.

void Stratergies::breakout()
{

}


