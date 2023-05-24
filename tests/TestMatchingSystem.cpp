#include <gtest/gtest.h>
#include <crypto-platform/MatchingSystem.h>

MatchSystem match;
// Ive defined this function outisde the test, is this okay??
std::vector<std::pair<OrderBookEntry, OrderBookEntry>> testMatch(bool size)
{
    match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");
    std::vector<OrderBookEntry> orders;
    if (size == true) 
    {
        OrderBookEntry entry("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1);
        orders.push_back(entry);
    }
    if (size == false)
    {
        OrderBookEntry entry("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,0,0);
        orders.push_back(entry);
    }

    std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matches = match.matchEngine(orders[0]);
    return matches;
}

TEST(MatchSystemTest, FileRead) {
    MatchSystem match;
    {
        // Correct amount of products to load in
        int count = match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");
        // We expcect count to be 3450
        EXPECT_EQ(count, 3540);
    }
}

TEST(MatchSystemTest, orderBookClear) {
    MatchSystem match;

    {
        // Load CSV into orderBook
        auto count = match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");

        EXPECT_EQ(count, 3540);
    }

    {
        // Clear the orderbook
        match.clearOrderBook();

        // Orderbook size should be empty (0)
        EXPECT_EQ(match.getOrderBook().size(), 0);
    }
}

TEST(MatchSystemTest, orderBookInsert) {
    MatchSystem match;
    {
        OrderBookEntry askEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
        // Add ask into OrderBook
        match.inserOrder("BTC/ETH", "ask", askEntry);
        EXPECT_EQ(match.getOrderBook()["BTC/ETH"]["orderType"]["ask"].size(), 1);
    }

    {
        OrderBookEntry bidEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,2);
        // Add bid into OrderBook
        match.inserOrder("BTC/ETH", "bid", bidEntry);
        EXPECT_EQ(match.getOrderBook()["BTC/ETH"]["orderType"]["bid"].size(), 1);
    }
}

TEST(MatchSystemTest, orderBookRemove) {
    MatchSystem match;
    {
        OrderBookEntry askEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
        // Add ask into OrderBook
        match.inserOrder("BTC/ETH", "ask", askEntry);
        // Remove ask from OrderBook
        match.removeOrder("BTC/ETH", "ask");
        // Ordersize should be 0
        EXPECT_EQ(match.getOrderBook()["BTC/ETH"]["orderType"]["ask"].size(), 0);
    }

    {
        OrderBookEntry bidEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,2);
        // Add bid into OrderBook
        match.inserOrder("BTC/ETH", "bid", bidEntry);
        // Remove ask from OrderBook
        match.removeOrder("BTC/ETH", "bid");
        EXPECT_EQ(match.getOrderBook()["BTC/ETH"]["orderType"]["bid"].size(), 0);
    }
}

TEST(MatchSystemTest, MatchFullOrders) {
    MatchSystem match;
    {
        // Large order, so there will be a match
        auto matches_1 = testMatch(false);

        // We would expcet to see no matches here
        EXPECT_EQ(matches_1.size(), 0);
    }

    {
        // Small order, so there will be no match
        auto matches_2 = testMatch(true);

        // We would expect to see 1 match here
        EXPECT_EQ(matches_2.size(), 1);
    }
}

TEST(MatchSystemTest, MatchPartialOrders) {
    MatchSystem match;

    // BIDS TO ASKS
    {
        // Create asks and bid
        OrderBookEntry askEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,1);
        OrderBookEntry askEntry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,1);
        OrderBookEntry bidEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,0.5);

        // Insert asks into orderbook
        match.inserOrder("BTC/ETH", "ask", askEntry1);
        match.inserOrder("BTC/ETH", "ask", askEntry2);

        // Match the bid to the asks
        auto matches = match.matchEngine(bidEntry1);

        // Expect to see 1 match from 0.5 BTC
        EXPECT_EQ(matches.size(), 1);
    }

    {
        // Create asks and bid
        OrderBookEntry askEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,1);
        OrderBookEntry askEntry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,1);
        OrderBookEntry bidEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,1.5);

        // Insert asks into orderbook
        match.inserOrder("BTC/ETH", "ask", askEntry1);
        match.inserOrder("BTC/ETH", "ask", askEntry2);

        // Match the bid to the asks
        auto matches = match.matchEngine(bidEntry1);

        // Expect to see 2 matches from 1.5 BTC
        EXPECT_EQ(matches.size(), 2);
    }

    // ASKS TO BIDS
    {
        // Create asks and bid
        OrderBookEntry bidEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,1);
        OrderBookEntry bidEntry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,1);
        OrderBookEntry askEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1.5,1);

        // Insert asks into orderbook
        match.inserOrder("BTC/ETH", "bid", bidEntry1);
        match.inserOrder("BTC/ETH", "bid", bidEntry2);

        // Match the bid to the asks
        auto matches = match.matchEngine(askEntry1);

        // Expect to see 1 match for 1.5 BTC for 1 ETH
        EXPECT_EQ(matches.size(), 0);
    }

    {
        // Create asks and bid
        OrderBookEntry bidEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,1);
        OrderBookEntry bidEntry2("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,1);
        OrderBookEntry askEntry1("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,0.5,1);

        // Insert asks into orderbook
        match.inserOrder("BTC/ETH", "bid", bidEntry1);
        match.inserOrder("BTC/ETH", "bid", bidEntry2);

        // Match the bid to the asks
        auto matches = match.matchEngine(askEntry1);

        // Expect to see 1 match from 1 BTC at 0.5 ETH
        EXPECT_EQ(matches.size(), 1);
    }
}

TEST(MatchSystemTest, TradeSystem) {
    MatchSystem match;
    {
        // Loading in the OrderBook
        match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");

        // Add User Ask entry
        OrderBookEntry entry("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1);

        // Match orders within order book
        auto matches = match.matchEngine(entry); 

        // Get number of trades
        auto trades = match.getTrades();

        // A single match witho a fulfilled rder should be 1
        EXPECT_EQ(trades.size(), 1);
    }

    {
        auto trades = match.getTrades();
        double sumBuyPrices = match.totalBuy(trades);
        double sumSellPrices = match.totalSell(trades);

        EXPECT_EQ(sumBuyPrices, 1);
        EXPECT_EQ(sumSellPrices, 1);
    }

}