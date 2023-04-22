#include <gtest/gtest.h>
#include <crypto-platform/OrderBook.h>

TEST(OrderBookTest, TestStats)
{
    double highPrice = 3;
    double lowPrice = 1;
    double avgPrice = 2;
    double spread = 2;
    OrderBook::OrderBookEntryList orders;
    OrderBookEntry entry1("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 1, 1);
    OrderBookEntry entry2("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 2, 1);
    OrderBookEntry entry3("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 3, 1);
    orders.push_back(entry1);
    orders.push_back(entry2);
    orders.push_back(entry3);

    EXPECT_EQ(OrderBook::getHighPrice(orders), highPrice);
    EXPECT_EQ(OrderBook::getLowPrice(orders), lowPrice);
    EXPECT_EQ(OrderBook::getHighPrice(orders) - OrderBook::getLowPrice(orders), spread);
    EXPECT_EQ(OrderBook::getAvgPrice(orders), avgPrice);
}

TEST(OrderBookTest, TestUncrossingOneSided)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 1, 1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 0);
}

TEST(OrderBookTest, TestUncrossingTwoSidedNoCross)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 1, 1});
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::ask, 3, 1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 0);
}

TEST(OrderBookTest, TestUncrossingTwoSidedOneCross)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 3, 1});
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::ask, 1, 1});
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::ask, 2, 1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 1);
}

TEST(OrderBookTest, TestUncrossingTwoSidedTwoCrosses)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 3, 2});
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::ask, 1, 1});
    orders.push_back({"2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::ask, 2, 1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 2);
}
