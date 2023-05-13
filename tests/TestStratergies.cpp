#include <gtest/gtest.h>
#include <crypto-platform/Strategies.h>
#include <crypto-platform/MatchingSystem.h>

TEST(StrategiesTest, FixedTradeSize) {
    Strategies strat;

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

TEST(StrategiesTest, PercentTradeSize) {
    Strategies strat;

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

TEST(StrategiesTest, AveragePrice) {
    MatchSystem match;
    Strategies strat;
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

TEST(StrategiesTest, StandardDeviation) {
    MatchSystem match;
    Strategies strat;
    {
        // Create 3 asks with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,2,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "ask", entry1);
        match.inserOrder("BTC/ETH", "ask", entry2);
        match.inserOrder("BTC/ETH", "ask", entry3);

        // Get the order book
        auto orders = match.getOrderBook()["BTC/ETH"]["orderType"]["ask"];

        // Standard deviation is calcualted and rounded to 2 decimal places
        EXPECT_EQ(strat.calcStandardDeviation(orders), 0.82);
    }

    match.clearOrderBook();

    {
        // Create 3 bids with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,2,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "bid", entry1);
        match.inserOrder("BTC/ETH", "bid", entry2);
        match.inserOrder("BTC/ETH", "bid", entry3);

        // Get the order book
        auto orders =match.getOrderBook()["BTC/ETH"]["orderType"]["bid"];

        // Standard deviation is calcualted and rounded to 2 decimal places
        EXPECT_EQ(strat.calcStandardDeviation(orders), 0.82);
    }
}

TEST(StrategiesTest, ResistanceLevel) {
    MatchSystem match;
    Strategies strat;
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

        // Standard deviation is calcualted and rounded to 2 decimal places
        EXPECT_EQ(strat.calcResistanceLevel(orders), 3);
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

        // Standard deviation is calcualted and rounded to 2 decimal places
        EXPECT_EQ(strat.calcResistanceLevel(orders), 3);
    }
}

TEST(StrategiesTest, MeanReversion) {
    MatchSystem match;
    Strategies strat;
    {
        // Create 3 asks with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,2,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "ask", entry1);
        match.inserOrder("BTC/ETH", "ask", entry2);
        match.inserOrder("BTC/ETH", "ask", entry3);

        // Get the order book
        auto orders = match.getOrderBook()["BTC/ETH"]["orderType"]["ask"];
        double avgPrice = strat.calcAveragePrice(orders);

        // AvgPrice is 2, so entry1 price is > than avg price so we sell
        EXPECT_EQ(strat.meanReversion(entry1, avgPrice),1);
        // AvgPrice is 2, so entry2 price is = to avg price so we hold
        EXPECT_EQ(strat.meanReversion(entry2, avgPrice),2);
        // AvgPrice is 2, so entry3 price is < than avg price so we buy
        EXPECT_EQ(strat.meanReversion(entry3, avgPrice),0);
    }

    match.clearOrderBook();

    {
        // Create 3 bids with a price of 3
        OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,2);
        OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,2,2);
        OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,3,2);

        // Add asks into OrderBook
        match.inserOrder("BTC/ETH", "bid", entry1);
        match.inserOrder("BTC/ETH", "bid", entry2);
        match.inserOrder("BTC/ETH", "bid", entry3);

        // Get the order book
        auto orders =match.getOrderBook()["BTC/ETH"]["orderType"]["bid"];
        double avgPrice = strat.calcAveragePrice(orders);
        double stdev = strat.calcStandardDeviation(orders);

        // AvgPrice is 2, so entry1 price is in the upper bounds so we buy
        EXPECT_EQ(strat.boundReversion(entry1.price, avgPrice, stdev), 1);
        // AvgPrice is 2, so entry1 price is = avg price
        EXPECT_EQ(strat.boundReversion(entry2.price, avgPrice,  stdev), 2);
        // AvgPrice is 2, so entry1 price is in the lower bounds so we sell
        EXPECT_EQ(strat.boundReversion(entry3.price, avgPrice,  stdev), 0);
    }
}

// TEST(StrategiesTest, BreakOut) {
//     MatchSystem match;
//     Strategies strat;
//     {
//         // Create 3 asks with a price of 3
//         OrderBookEntry entry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
//         OrderBookEntry entry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,2,2);
//         OrderBookEntry entry3("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,3,2);

//         // Add asks into OrderBook
//         match.inserOrder("BTC/ETH", "ask", entry1);
//         match.inserOrder("BTC/ETH", "ask", entry2);
//         match.inserOrder("BTC/ETH", "ask", entry3);

//         // Get the order book
//         auto orders = match.getOrderBook()["BTC/ETH"]["orderType"]["ask"];
//         double resistance = strat.calcResistanceLevel(orders);

//         // AvgPrice is 2, so entry1 price is > than avg price so we sell
//         EXPECT_EQ(strat.breakOut(orders, resistance), 2)
//         << "Resistsnce: " << resistance;
//         // AvgPrice is 2, so entry2 price is = to avg price so we hold
//         EXPECT_EQ(strat.breakOut(orders, resistance), 0);
//     }
// }
