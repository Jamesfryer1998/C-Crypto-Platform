#include <gtest/gtest.h>
#include <crypto-platform/Stratergies.h>
#include <crypto-platform/MatchingSystem.h>

TEST(StratergiesTest, FixedTradeSize) {
    Stratergies strat;

    {
        // Generate trade size from function using 1000 trades and units of 10 currency
        double tradeSize = strat.fixedTradeSize(1000, 10);

        // 10/1000 is equal to 0.01
        EXPECT_EQ(tradeSize, 0.01);
    }

    {
        // Generate trade size from function using 1000 trades and units of 10 currency
        double tradeSize = strat.fixedTradeSize(1000, 10);

        // 10/1000 is equal to 0.01 not 0.02. We assert to not equal to
        ASSERT_NE(tradeSize, 0.02);
    }
}

TEST(StratergiesTest, PercentTradeSize) {
    Stratergies strat;

    {
        // Generate trade size form function based on 10%
        double tradeSize = strat.percentTradeSize(10, 10);

        // 10 * 10 / 100 is equal to 1
        EXPECT_EQ(tradeSize, 1);
    }

    {
        // Generate trade size form function based on 50%
        double tradeSize = strat.percentTradeSize(50, 10);

        // 10 * 50 / 100 is equal to 5
        EXPECT_EQ(tradeSize, 5);
    }

    {
        // Generate trade size form function based on 0%
        double tradeSize = strat.percentTradeSize(0, 10);

        // 10 * 0 / 100 is equal to 0
        EXPECT_EQ(tradeSize, 0);
    }

    {
        // Generate trade size form function based on 100%
        double tradeSize = strat.percentTradeSize(100, 10);

        // 10 * 100 / 100 is equal to 10
        EXPECT_EQ(tradeSize, 10);
    }
}

// TEST(StratergiesTest, VolatilityTradeSize) {
//     Stratergies strat;

//     {
//         double prices[] = {100.0, 105.0, 110.0, 115.0, 120.0};
//         double volatility = strat.volatilityTradeSize(prices, 5);
//         double trade_size_multiplier = strat.calculate_trade_size_multiplier(volatility);
//         double base_trade_size = 100.0;
//         double adjusted_trade_size = strat.adjust_trade_size(base_trade_size, trade_size_multiplier);

//         EXPECT_EQ(adjusted_trade_size, 107.071);

//     }
    
// }

TEST(StratergiesTest, AveragePrice) {
    MatchSystem match;
    Stratergies strat;
    {
        // Create 3 asks with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "ask", entry1);
        match.inserOrder("BTC/ETH", "ask", entry2);
        match.inserOrder("BTC/ETH", "ask", entry3);

        // Get the order book
        auto orders = match.getOrderBook()["BTC/ETH"]["orderType"]["ask"];

        // Total price of 9, 9/3 = 3
        EXPECT_EQ(strat.calcAveragePrice(orders), 3);
    }

    match.clearOrderBook();

    {
        // Create 3 bids with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,3,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,3,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "bid", entry1);
        match.inserOrder("BTC/ETH", "bid", entry2);
        match.inserOrder("BTC/ETH", "bid", entry3);

        // Get the order book
        auto orders =match.getOrderBook()["BTC/ETH"]["orderType"]["bid"];

        // Total price of 9, 9/3 = 3
        EXPECT_EQ(strat.calcAveragePrice(orders), 3);
    }
}
