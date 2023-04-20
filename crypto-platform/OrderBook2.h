#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <chrono>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace OrderBook2
{

enum class Side{bid, ask};
using OrderID = uint64_t;
using Price = double;
using OptionalPrice = std::optional<Price>;
using Qty = double;
using Client = std::string;
using Timestamp = std::chrono::nanoseconds; 

struct Order
{
    OrderID id;
    Side side;
    Price price;
    Qty amount;
    Timestamp timestamp;
    Client client;
};


struct Trade
{
    Side passiveSide;
    double price;
    double amount;
    uint64_t timestamp;
    std::string passiveClient;
    std::string aggressingClient;
};

using Trades = std::vector<Trade>;

using BBO = std::pair<OptionalPrice, OptionalPrice>;

class OrderBook
{
public:
    auto find(OrderID id) const
    {
        return orders.find(id);
    }

    auto end() const
    {
        return orders.end();
    }

    BBO getBBO() const
    {
        auto insideBid {std::begin(bids)};
        auto insideAsk {std::begin(asks)};

        return BBO{
                insideBid == std::end(bids) ? OptionalPrice{std::nullopt}: OptionalPrice{insideBid->first},
                insideAsk == std::end(asks) ? OptionalPrice{std::nullopt} : OptionalPrice{insideAsk->first}
                };
    }

    bool isEmpty() const
    {
        return bids.size() == 0 && asks.size() == 0;
    }

    bool isOneSidedOrEmpty() const
    {
        return bids.size() == 0 || asks.size() == 0;
    }

    bool isCrossed() const
    {
        auto bbo {getBBO()};
        return bbo.first.has_value() && bbo.second.has_value() && bbo.first >= bbo.second;

    }

    bool isLocked() const
    {
        auto bbo {getBBO()};
        return bbo.first.has_value() && bbo.second.has_value() && bbo.first == bbo.second;

    }

    bool hasPriceLevel(Side side, Price price) const
    {
        if (side == Side::bid)
            return bids.find(price) != bids.end();
        else
            return asks.find(price) != asks.end();
    }

    OrderID insert(Side side, Price price, Qty amount, Timestamp timestamp, const Client& client)
    {
        OrderID id{nextID++};

        Order& order = orders[id];
        order = Order {
            .id = id,
            .side = side,
            .price = price,
            .amount = amount,
            .timestamp = timestamp,
            .client = client
        };

        Trades trades;

        if (order.side == Side::bid)
        {
            // Sweep ask book with this order creating trades
            while (true)
            {
                // No orders on side
                if (!asks.size()) break;
                auto& insideAsk = asks.begin()->second;
                Price insideAskPrice = asks.begin()->first;
                // Inside ask price is more the order is prepared to pay
                if (insideAskPrice > order.price) break;
                Order* passive {*insideAsk.begin()};
                Qty matchAmount {std::min(order.amount, passive->amount)};
                order.amount -= matchAmount;
                passive->amount -= matchAmount;
                if (order.amount == 0) {}
                    // Done
                if (passive->amount == 0) {}
                    // Cancel order
                    // Go round again
            }
            if (order.amount > 0)
                insertIntoPriceLevel(bids, order);
        }
        else
        {
            insertIntoPriceLevel(asks, order);
        }

        return order.id;
    }


    bool cancel(OrderID id)
    {
        auto orderIter{find(id)};
        if (orderIter == end())
            return false;
        bool result {
            orderIter->second.side == Side::bid
                ? cancelFromPriceLevel(bids, orderIter->second.price, orderIter->second.id)
                : cancelFromPriceLevel(asks, orderIter->second.price, orderIter->second.id)};
        orders.erase(orderIter);
        return result;
    }

    void dump() const
    {
        for(const auto& pair: orders)
        {
            std::cout << pair.first << " " << pair.second.id << std::endl;
        }
    }

private:
    using OrderQueue = std::list<Order*>;
    using Orders = std::map<OrderID, Order>;
    using Bids = std::map<double, OrderQueue, std::greater<double>>;
    using Asks = std::map<double, OrderQueue, std::less<double>>;

    void insertIntoPriceLevel(auto& ordersByPrice, Order& order)
    {
        OrderQueue& priceLevel = ordersByPrice[order.price];
        priceLevel.push_back(&order);
    }

    bool cancelFromPriceLevel(auto& ordersByPrice, Price price, OrderID id)
    {
        OrderQueue& priceLevel = ordersByPrice[price];
        if (priceLevel.size() == 1)
        {
            ordersByPrice.erase(price);
            return true;
        }
        else
            return std::erase_if(priceLevel, [id](const auto& value) { return value->id == id; });
    }

    // The next order-id we will assign to a new order
    OrderID nextID{1};
    // List of bid price levels in price level order
    Bids bids;
    // List of ask price levels in price level order
    Asks asks;
    // List of orders - this manages order lifetimes
    Orders orders;
};
};