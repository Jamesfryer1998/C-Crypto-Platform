#include <numeric>
#include <iostream>
#include <crypto-platform/Strategies.h>

Strategies::Strategies()
{

}

double Strategies::fixedTradeSize(int numtrades, double currencyAmount)
{
    return currencyAmount/numtrades;
}

double Strategies::percentTradeSize(int percent, double currencyAmount)
{
    return currencyAmount * percent / 100;
}

double Strategies::volatilityTradeSize(const double* prices, int num_prices)
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

double Strategies::calculate_trade_size_multiplier(double volatility) 
{
    // Calculate the trade size multiplier based on volatility
    return 1 + (volatility / 100);
}

double Strategies::adjust_trade_size(double base_trade_size, double trade_size_multiplier) 
{
    // Adjust the trade size based on the current volatility multiplier
    return base_trade_size * trade_size_multiplier;
}

double Strategies::calcAveragePrice(const std::vector<OrderBookEntry>& orders)
{
    double sum = std::accumulate(orders.begin(), orders.end(), 0.0, [](double a, const OrderBookEntry& b) {
        return a + b.price;
    });
    return sum / orders.size();
}

// double Stratergies::calcStandardDeviation(std::vector<OrderBookEntry>& orders)
// {

// }

double Strategies::calcStandardDeviation(const std::vector<OrderBookEntry>& entries) 
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

double Strategies::calcResistanceLevel(const std::vector<OrderBookEntry>& orders)
{
    double mean = calcAveragePrice(orders);
    double stdev = calcStandardDeviation(orders);
    double resistance = mean + 2 * stdev;
    return resistance;
}

/* Mean reversion staratergy
0 = sell
1 = buy
2 = hold*/
int Strategies::meanReversion(double currentPrice, double histAveragePrice)
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

int Strategies::boundReversion(double currentPrice, double histAveragePrice, double stdev) 
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
// of the breakout when the price breaks through that level.

int Strategies::breakOut(const std::vector<OrderBookEntry>& entries, double resistance) {
    // Check if the current price has broken through the resistance level
    double current_price = entries.back().price;
    if (current_price > resistance) {
        return 1;
    } else {
        return 0;
    }
}


// Used to generate a ordebookentry from the product, time stamp, price, ask and amount:
// Timestamp - determined from the current timestamp
// Product - determined from the current product in currMap
// Price - From the lowest ask price or from the average price (not sure)
// Ask - as were generating a ask order
// Amount - Determined from one of the trade amount methods chosen by the User
// OrderBookEntry Stratergies::generateAsk()
// {

// }

// OrderBookEntry Stratergies::generateBid()
// {
    
// }


