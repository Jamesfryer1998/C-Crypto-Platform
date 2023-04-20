#include <gtest/gtest.h>
#include <crypto-platform/OrderBook.h>
#include <crypto-platform/OrderBook2.h>

TEST(OrderBookTest, TestStats)
{
    double highPrice = 3;
    double lowPrice = 1;
    double avgPrice = 2;
    double spread = 2;
    OrderBook::OrderBookEntryList orders;
    OrderBookEntry entry1("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1);
    OrderBookEntry entry2("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,2,1);
    OrderBookEntry entry3("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,3,1);
    orders.push_back(entry1);
    orders.push_back(entry2);
    orders.push_back(entry3);

    std::cout << "\nTests:" << std::endl;

    EXPECT_EQ(OrderBook::getHighPrice(orders), highPrice);
    EXPECT_EQ(OrderBook::getLowPrice(orders), lowPrice);
    EXPECT_EQ(OrderBook::getHighPrice(orders) - OrderBook::getLowPrice(orders), spread);
    EXPECT_EQ(OrderBook::getAvgPrice(orders), avgPrice);
}


TEST(OrderBookTest, TestUncrossingOneSided)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 0);
}


TEST(OrderBookTest, TestUncrossingTwoSidedNoCross)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1});
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::ask,3,1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 0);
}

TEST(OrderBookTest, TestUncrossingTwoSidedOneCross)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,3,1});
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::ask,1,1});
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::ask,2,1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 1);
}

TEST(OrderBookTest, TestUncrossingTwoSidedTwoCrosses)
{
    OrderBook::OrderBookEntryList orders;
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,3,2});
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::ask,1,1});
    orders.push_back({"2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::ask,2,1});

    OrderBook orderBook(orders);

    auto matches = orderBook.matchSystem("ETH/BTC", "2020/03/17 17:01:24.884492");
    EXPECT_EQ(matches.size(), 2);
}

TEST(OrderBook2Test, TestOrderAdd)
{
    OrderBook2::OrderBook orderBook;
    OrderBook2::OrderID id{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    orderBook.dump();
    EXPECT_FALSE(orderBook.find(id) == orderBook.end());
    EXPECT_TRUE(orderBook.find(2) == orderBook.end());
}

TEST(OrderBook2Test, TestBBO)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // ----
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    // When we get BBO the BBO is {3.0, 0.0}
    EXPECT_EQ(orderBook.getBBO(), std::make_pair(OrderBook2::OptionalPrice{3}, OrderBook2::OptionalPrice{std::nullopt}));

    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // 1@4.0
    // ----
    // When we get BBO the BBO is {3.0, 4.0}
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    EXPECT_EQ(orderBook.getBBO(), std::make_pair(OrderBook2::OptionalPrice{3}, OrderBook2::OptionalPrice{4}));

    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // 1@4.0
    // 1@5.0
    // ----
    // When we get BBO the BBO is {3.0, 4.0}
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{5}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    EXPECT_EQ(orderBook.getBBO(), std::make_pair(OrderBook2::OptionalPrice{3}, OrderBook2::OptionalPrice{4}));

    // Given
    // BIDS
    // 1@2.0
    // 1@3.0
    // ASKS
    // 1@4.0
    // 1@5.0
    // ----
    // When we get BBO the BBO is {3.0, 4.0}
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{2}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    EXPECT_EQ(orderBook.getBBO(), std::make_pair(OrderBook2::OptionalPrice{3}, OrderBook2::OptionalPrice{4}));
}

TEST(OrderBook2Test, TestCrossed)
{
    OrderBook2::OrderBook orderBook;
    // Given an empty book
    // When we check then the book is not crossed or locked
    EXPECT_FALSE(orderBook.isCrossed());
    EXPECT_FALSE(orderBook.isLocked());
    EXPECT_TRUE(orderBook.isOneSidedOrEmpty());

    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // ----
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    // When we check then the book is not crossed or locked and it's once sided
    EXPECT_FALSE(orderBook.isCrossed());
    EXPECT_FALSE(orderBook.isLocked());
    EXPECT_TRUE(orderBook.isOneSidedOrEmpty());
    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // 1@3.0
    // ----
    // When we check then the book is crossed and locked and not empty or one sided
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    EXPECT_TRUE(orderBook.isCrossed());
    EXPECT_TRUE(orderBook.isLocked());
    EXPECT_FALSE(orderBook.isOneSidedOrEmpty());

    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // 1@2.5
    // 1@3.0
    // ----
    // When we check then the book is crossed but not locked
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{2.5}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    EXPECT_TRUE(orderBook.isCrossed());
    EXPECT_FALSE(orderBook.isLocked());
}


TEST(OrderBook2Test, TestOrderCancel)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // 1@3.0, 1@3.0
    // ASKS
    // ----
    OrderBook2::OrderID id1{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    OrderBook2::OrderID id2{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    // When we cancel one order
    orderBook.cancel(id1);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(id1) == orderBook.end());
    // and there's no price level
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
    // When we cancel the last order
    orderBook.cancel(id2);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(id2) == orderBook.end());
    // and there's no price level
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}