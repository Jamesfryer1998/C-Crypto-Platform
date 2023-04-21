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
    OrderBookEntry entry1("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 1, 1);
    OrderBookEntry entry2("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 2, 1);
    OrderBookEntry entry3("2020/03/17 17:01:24.884492", "ETH/BTC", OrderBookType::bid, 3, 1);
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

TEST(OrderBook2Test, TestOrderAdd)
{
    OrderBook2::OrderBook orderBook;
    auto [id, fullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
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
    auto [id, fullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    // When we get BBO the BBO is {3.0, 0.0}
    EXPECT_EQ(orderBook.getBBO(), std::make_pair(OrderBook2::OptionalPrice{3}, OrderBook2::OptionalPrice{std::nullopt}));
    const OrderBook2::Order &order{orderBook.find(id)->second};
    EXPECT_EQ(order.id, id);
    EXPECT_EQ(order.price, OrderBook2::Price{3});
    EXPECT_EQ(order.amount, OrderBook2::Qty{1});
    EXPECT_EQ(order.timestamp, OrderBook2::Timestamp{0});
    EXPECT_EQ(order.client, "test");

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

    orderBook.dump();
}

TEST(OrderBook2Test, TestOrderCancel)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // 1@3.0, 1@3.0
    // ASKS
    // ----
    auto [id1, id1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    auto [id2, id2FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    // When we cancel one order
    orderBook.cancel(id1);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(id1) == orderBook.end());
    // and there's a price level
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
    // When we cancel the last order
    orderBook.cancel(id2);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(id2) == orderBook.end());
    // and there's no price level
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST(OrderBook2Test, TestSingleFullFillBid)
{
    OrderBook2::OrderBook orderBook;
    // Given

    // BIDS
    // ASKS
    // 1@3.0, 10@3.0
    // ----
    auto [passive1, passive1FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "passive")};
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");

    // When we cross the ASK with 1@3
    auto [cross1, cross1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then we get one trade which is cross1 crossing passive1
    const OrderBook2::OrderBook::Trades &trades = orderBook.getLastTrades();
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].orderID, passive1);
    // ... and the crossing order is fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... and both the crossing order and passive1 order are gone
    std::cout << cross1 << std::endl;
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // and the ASK still has quantity at 3.
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST(OrderBook2Test, TestSingleFullFillAsk)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // 1@3.0, 10@3.0
    // ASKS
    // ----
    auto [passive1, passive1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "passive")};
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");
    // When we cross 1@3
    auto [cross1, cross1FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then we get one trade which is cross1 crossing passive1
    const OrderBook2::OrderBook::Trades &trades = orderBook.getLastTrades();
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].orderID, passive1);
    // ... and the crossing order is fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... and both the crossing order and passive1 order are gone
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // and the BID still has quantity at 3.
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST(OrderBook2Test, TestPartialPassiveFillBid)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // ASKS
    // 30@4.0
    // 10@3.0
    // ----
    auto [passive1, passive1FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};
    // When we cross with a BID of 15@4
    auto [cross1, cross1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};

    // the cross1 order fully crosses passive1 with 10@4.0 and partially crosses passive2 for 5@4.0.
    const OrderBook2::OrderBook::Trades &trades = orderBook.getLastTrades();
    EXPECT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].orderID, passive1);
    EXPECT_EQ(trades[0].amount, 10);
    EXPECT_EQ(trades[0].price, 3);
    EXPECT_EQ(trades[1].orderID, passive2);
    EXPECT_EQ(trades[1].amount, 5);
    EXPECT_EQ(trades[1].price, 4);

    // ... and the cross1 order is fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... and the crossing order is fully filled
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    // ... and passive1 is fully filled
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // ... and passive2 is partially filled
    EXPECT_FALSE(orderBook.find(passive2) == orderBook.end());
    // and the ASK still has quantity at 4
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::ask, OrderBook2::Price{3}));
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::ask, OrderBook2::Price{4}));
}

TEST(OrderBook2Test, TestPartialPassiveFillAsk)
{
    std::cout << "TestPartialPassiveFillAsk" << std::endl;
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // 30@3.0
    // 10@4.0
    // ASKS
    // ----
    auto [passive1, passive1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};
    // When we cross the BID with a bid of 15@3
    auto [cross1, cross1FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then the crossing order cross1 fully crosses passive1 and and partially crosses passive2
    const OrderBook2::OrderBook::Trades &trades = orderBook.getLastTrades();
    EXPECT_EQ(trades.size(), 2);
    EXPECT_EQ(trades[0].orderID, passive1);
    EXPECT_EQ(trades[0].amount, 10);
    EXPECT_EQ(trades[0].price, 4);
    EXPECT_EQ(trades[1].orderID, passive2);
    EXPECT_EQ(trades[1].amount, 5);
    EXPECT_EQ(trades[1].price, 3);

    // ... the crossing order has been fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... so the crossing order is not on the book
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    // ... and passive1 is not on the book
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // ... and passive2 is still there
    EXPECT_FALSE(orderBook.find(passive2) == orderBook.end());
    // ... and there's only quantity at 3 now.
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{4}));

    std::cout << "TestPartialPassiveFillAsk" << std::endl;
}

TEST(OrderBook2Test, TestPartialAggressingFill)
{
    OrderBook2::OrderBook orderBook;
    // Given
    // BIDS
    // ASKS
    // 10@3.0, 30@3.0
    // ----
    auto [passive1, passive1FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};
    // When we cross the ASK with 15@3
    auto [cross1, cross1FullyFilled]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};
    // Then passive1 is fully filled and the crossing order is partially filled
    const OrderBook2::OrderBook::Trades &trades = orderBook.getLastTrades();
    EXPECT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].orderID, passive1);
    EXPECT_EQ(trades[0].amount, 10);
    EXPECT_EQ(trades[0].price, 3);
    // ... and the crossing order has not been fully filled
    EXPECT_FALSE(cross1FullyFilled);
    // ... and the crossing order remains on the book
    EXPECT_FALSE(orderBook.find(cross1) == orderBook.end());
    // ... and passive1 is no longer on the book
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // ... and passive2 remains on the book
    EXPECT_FALSE(orderBook.find(passive2) == orderBook.end());
    // ... and price level 3 stays on the book because cross1's remaing qty remains there
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::ask, OrderBook2::Price{3}));
    // ... and price level 3 is still on the book
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::ask, OrderBook2::Price{4}));
    // ... and the crossing order's qty and price are correct
    const OrderBook2::Order &crossingOrder{orderBook.find(cross1)->second};
    EXPECT_EQ(crossingOrder.id, cross1);
    EXPECT_EQ(crossingOrder.price, 3);
    EXPECT_EQ(crossingOrder.amount, 5);
    // ... and the order is now on the other side of the book.
    EXPECT_EQ(crossingOrder.side, OrderBook2::Side::ask);
}