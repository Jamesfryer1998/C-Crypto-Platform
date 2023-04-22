#include <gtest/gtest.h>
#include <crypto-platform/OrderBook2.h>

// Testing fixture
class OrderBook2Test : public ::testing::Test
{
    void SetUp() override {}
    void TearDown() override {}
protected:
    void clearOrderBook() { orderBook.clear(); }
    OrderBook2::OrderBook orderBook;
};

TEST_F(OrderBook2Test, TestOrderAdd)
{
    auto [id, fullyFilled, trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    EXPECT_FALSE(orderBook.find(id) == orderBook.end());
    EXPECT_TRUE(orderBook.find(2) == orderBook.end());
    EXPECT_EQ(trades.size(), 0);
}

TEST_F(OrderBook2Test, TestBBO)
{
    // Given an empty order book
    // When we get the BBO
    // Then we expect it to be empty
    EXPECT_EQ(orderBook.getBBO(), OrderBook2::BBO(std::nullopt, std::nullopt));

    // Given
    // BIDS
    // 1@3.0
    // ASKS
    // ----
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    // When we get BBO the BBO is {3.0, <empty>}
    EXPECT_EQ(orderBook.getBBO(), OrderBook2::BBO(3, std::nullopt));

    // Given
    // BIDS
    // ASKS
    // 1@4.0
    // ----
    clearOrderBook();

    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
    // When we get BBO the BBO is {3.0, <empty>}
    EXPECT_EQ(orderBook.getBBO(), OrderBook2::BBO(std::nullopt, 4));

    // Given
    // BIDS
    // 3@4.0
    // ASKS
    // 1@4.0
    // ----
    // When we get BBO the BBO is {3.0, 4.0}
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test");
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

TEST_F(OrderBook2Test, TestOrderCancel)
{
    // Given
    // BIDS
    // 1@3.0, 1@3.0
    // ASKS
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    auto [passive2, passive2FullyFilled, passive2Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "test")};
    // When we cancel one order
    orderBook.cancel(passive1);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // and there's a price level
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
    // When we cancel the last order
    orderBook.cancel(passive2);
    // Then we cannot find it again
    EXPECT_TRUE(orderBook.find(passive2) == orderBook.end());
    // and there's no price level
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST_F(OrderBook2Test, TestSingleFullFillBid)
{
    OrderBook2::OrderBook orderBook;
    // Given

    // BIDS
    // ASKS
    // 1@3.0, 10@3.0
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "passive")};
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");

    // When we cross the ASK with 1@3
    auto [cross1, cross1FullyFilled, cross1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then we get one trade which is cross1 crossing passive1
    EXPECT_EQ(cross1Trades.size(), 1);
    EXPECT_EQ(cross1Trades[0].orderID, passive1);
    // ... and the crossing order is fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... and both the crossing order and passive1 order are gone
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // and the ASK still has quantity at 3.
    EXPECT_FALSE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST_F(OrderBook2Test, TestSingleFullFillAsk)
{
    // Given
    // BIDS
    // 1@3.0, 10@3.0
    // ASKS
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "passive")};
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");
    // When we cross 1@3
    auto [cross1, cross1FullyFilled, cross1Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{1}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then we get one trade which is cross1 crossing passive1
    EXPECT_EQ(cross1Trades.size(), 1);
    EXPECT_EQ(cross1Trades[0].orderID, passive1);
    EXPECT_EQ(cross1Trades[0].amount, 1);
    EXPECT_EQ(cross1Trades[0].price, 3);
    // ... and the crossing order is fully filled
    EXPECT_TRUE(cross1FullyFilled);
    // ... and both the crossing order and passive1 order are gone
    EXPECT_TRUE(orderBook.find(cross1) == orderBook.end());
    EXPECT_TRUE(orderBook.find(passive1) == orderBook.end());
    // and the BID still has quantity at 3.
    EXPECT_TRUE(orderBook.hasPriceLevel(OrderBook2::Side::bid, OrderBook2::Price{3}));
}

TEST_F(OrderBook2Test, TestPartialPassiveFillBid)
{
    // Given
    // BIDS
    // ASKS
    // 30@4.0
    // 10@3.0
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled, passive2Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};
    // When we cross with a BID of 15@4
    auto [cross1, cross1FullyFilled, cross1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};

    // the cross1 order fully crosses passive1 with 10@4.0 and partially crosses passive2 for 5@4.0.
    EXPECT_EQ(cross1Trades.size(), 2);
    EXPECT_EQ(cross1Trades[0].orderID, passive1);
    EXPECT_EQ(cross1Trades[0].amount, 10);
    EXPECT_EQ(cross1Trades[0].price, 3);
    EXPECT_EQ(cross1Trades[1].orderID, passive2);
    EXPECT_EQ(cross1Trades[1].amount, 5);
    EXPECT_EQ(cross1Trades[1].price, 4);

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

TEST_F(OrderBook2Test, TestPartialPassiveFillAsk)
{
    // Given
    // BIDS
    // 30@3.0
    // 10@4.0
    // ASKS
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled, passive2Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};

    // When we cross the BID with a bid of 15@3
    auto [cross1, cross1FullyFilled, cross1Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};

    // Then the crossing order cross1 fully crosses passive1 and and partially crosses passive2
    EXPECT_EQ(cross1Trades.size(), 2);
    EXPECT_EQ(cross1Trades[0].orderID, passive1);
    EXPECT_EQ(cross1Trades[0].amount, 10);
    EXPECT_EQ(cross1Trades[0].price, 4);
    EXPECT_EQ(cross1Trades[1].orderID, passive2);
    EXPECT_EQ(cross1Trades[1].amount, 5);
    EXPECT_EQ(cross1Trades[1].price, 3);

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
}

TEST_F(OrderBook2Test, TestPartialAggressingFill)
{
    // Given
    // BIDS
    // ASKS
    // 10@3.0, 30@3.0
    // ----
    auto [passive1, passive1FullyFilled, passive1Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{3}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive")};
    auto [passive2, passive2FullyFilled, passive2Trades]{orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{4}, OrderBook2::Qty{30}, OrderBook2::Timestamp{0}, "passive")};
    // When we cross the ASK with 15@3
    auto [cross1, cross1FullyFilled, cross1Trades]{orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{3}, OrderBook2::Qty{15}, OrderBook2::Timestamp{0}, "aggressive")};
    // Then passive1 is fully filled and the crossing order is partially filled
    EXPECT_EQ(cross1Trades.size(), 1);
    EXPECT_EQ(cross1Trades[0].orderID, passive1);
    EXPECT_EQ(cross1Trades[0].amount, 10);
    EXPECT_EQ(cross1Trades[0].price, 3);
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

TEST_F(OrderBook2Test, TestSideIteration)
{
    // Given
    // BIDS
    // 30@3.0
    // 10@4.0
    // ASKS
    // ----
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{5}, OrderBook2::Timestamp{0}, "passive");
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{2}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");
    orderBook.insert(OrderBook2::Side::bid, OrderBook2::Price{4}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{5}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");
    orderBook.insert(OrderBook2::Side::ask, OrderBook2::Price{6}, OrderBook2::Qty{10}, OrderBook2::Timestamp{0}, "passive");

    auto bidIter {std::begin(orderBook.getBids())};
    EXPECT_EQ((*bidIter)->price, 4);
    EXPECT_EQ((*bidIter)->amount, 5);
    bidIter++;
    EXPECT_EQ((*bidIter)->price, 4);
    EXPECT_EQ((*bidIter)->amount, 10);
    bidIter++;
    EXPECT_EQ((*bidIter)->price, 2);
    EXPECT_EQ((*bidIter)->amount, 10);
    auto askIter {std::begin(orderBook.getAsks())};
    EXPECT_EQ((*askIter)->price, 5);
    EXPECT_EQ((*askIter)->amount, 10);
    askIter++;
    EXPECT_EQ((*askIter)->price, 6);
    EXPECT_EQ((*askIter)->amount, 10);
}