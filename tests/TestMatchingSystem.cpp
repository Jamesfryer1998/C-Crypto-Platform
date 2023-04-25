#include <gtest/gtest.h>
#include <crypto-platform/MatchingSystem.h>

MatchSystem match;
int count = match.readCSV_NEW("/Users/james/Projects/C++ Crypto Trading/testing_suite/crypto_platform/20200317.csv");

// Ive defined this function outisde the test, is this okay??
std::vector<std::pair<OrderBookEntry, OrderBookEntry>> testMatch(bool size)
{
    match.readCSV_NEW("20200317.csv");
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
        // We expcect count to be 3450
        EXPECT_EQ(count, 3540);
    }
}

TEST(MatchSystemTest, MatchOrders) {
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

TEST(MatchSystemTest, orderBook) {
    MatchSystem match;
    auto orderBook = match.getOrderBook();

    {
        OrderBookEntry askEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::ask,1,2);
        // Add ask into OrderBook
        orderBook["BTC/ETH"]["orderType"]["ask"].push_back(askEntry);

        EXPECT_EQ(orderBook["BTC/ETH"]["orderType"]["ask"].size(), 1);
    }

    {
        OrderBookEntry bidEntry("2020/03/17 17:01:24.884492","BTC/ETH",OrderBookType::bid,1,2);

        // Add bid into OrderBook
        orderBook["BTC/ETH"]["orderType"]["bid"].push_back(bidEntry);

        EXPECT_EQ(orderBook["BTC/ETH"]["orderType"]["bid"].size(), 1);
    }

        {
        // Remove ask from OrderBook
        orderBook["BTC/ETH"]["orderType"]["ask"].erase(orderBook["BTC/ETH"]["orderType"]["ask"].begin());

        EXPECT_EQ(orderBook["BTC/ETH"]["orderType"]["ask"].size(), 0);
    }

    {
        
        // Add bid into OrderBook
        orderBook["BTC/ETH"]["orderType"]["bid"].erase(orderBook["BTC/ETH"]["orderType"]["bid"].begin());

        EXPECT_EQ(orderBook["BTC/ETH"]["orderType"]["bid"].size(), 0);
    }
}

// TODO:
//  Look at andys TestOrderBook2 test cases to see some of the schenarios to test from