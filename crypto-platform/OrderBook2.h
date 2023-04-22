#pragma once
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>
// I want ranges but join_view is marked as experimental due to pending C++23 ABI break,
// I can't turn on experimental everywhere because <chrono> does not compile.
#define _LIBCPP_ENABLE_EXPERIMENTAL
#include <ranges>
#undef _LIBCPP_ENABLE_EXPERIMENTAL
#include <list>
#include <map>

namespace OrderBook2 {

enum class Side
{
    bid,
    ask
};

inline std::ostream& operator<<(std::ostream& o, Side side)
{
    return o << ((side == Side::bid) ? "B" : "A");
}

inline constexpr Side oppositeSide(Side side)
{
    return side == Side::bid ? Side::ask : Side::bid;
}

using OrderID = uint64_t;
using ExecID = uint64_t;
using Price = double;
using OptionalPrice = std::optional<Price>;
using Qty = double;
using Client = std::string;
using Timestamp = std::chrono::nanoseconds;

struct Order
{
    Order(OrderID id, Side side, Price price, Qty amount, Client client, Timestamp timestamp)
      : id(id)
      , side(side)
      , price(price)
      , amount(amount)
      , client(client)
      , timestamp(timestamp)
    {
    }
    OrderID id;
    Side side;
    Price price;
    Qty amount;
    Client client;
    Timestamp timestamp;
};

inline std::ostream& operator<<(std::ostream& o, const Order& order)
{
    return o << order.id << "," << order.side << "," << order.price << "," << order.amount;
}

struct Trade
{
    Trade(OrderID orderID, ExecID execID, Side passiveSide, Price price, Qty amount)
      : orderID(orderID)
      , execID(execID)
      , passiveSide(passiveSide)
      , price(price)
      , amount(amount)
    {
    }

    OrderID orderID;
    ExecID execID;
    Side passiveSide;
    Price price;
    Qty amount;
};

using BBO = std::pair<OptionalPrice, OptionalPrice>;

class OrderBook
{
public:
    static constexpr OrderID FULLY_FILLED_ORDER_ID{ 0 };

    // find order, if it doesn't exist returns end()
    auto find(OrderID id) const { return orders.find(id); }

    // end of orders collection
    auto end() const { return orders.end(); }

    // Return a pair (bid-price, ask-price). Price is an OptionalPrice.
    BBO getBBO() const
    {
        auto insideBid{ std::begin(bids) };
        auto insideAsk{ std::begin(asks) };

        return BBO{ insideBid == std::end(bids) ? OptionalPrice{ std::nullopt } : OptionalPrice{ insideBid->first },
                    insideAsk == std::end(asks) ? OptionalPrice{ std::nullopt } : OptionalPrice{ insideAsk->first } };
    }

    // True if empty
    bool isEmpty() const { return bids.size() == 0 && asks.size() == 0; }

    // True if the book is one sided or empty
    bool isOneSidedOrEmpty() const { return bids.size() == 0 || asks.size() == 0; }

    // True if the book has the price level on the side
    bool hasPriceLevel(Side side, Price price) const
    {
        if (side == Side::bid)
            return bids.find(price) != bids.end();
        else
            return asks.find(price) != asks.end();
    }
    using Trades = std::vector<Trade>;

    // Insert a new order, may cross the book and generate trades.
    // Returns The orderID and whether it's a fully filled order.
    // Note:(1) price-time priority is not impacted by the timestamp, any order inserted
    //          after another one has lower time priority. The timestamp is simply a record.
    //     :(2) Orders that are fully filled are implicitly cancelled and removed from the book.
    std::tuple<OrderID, bool, Trades> insert(Side side, Price price, Qty amount, Timestamp timestamp, const Client& client)
    {
        OrderID id{ nextID++ };
        assert(!orders.contains(id));
        auto [it, _]{ orders.try_emplace(/* key */ id, /* order */ id, side, price, amount, client, timestamp) };
        auto& aggressingOrder{ it->second };

        auto [fullyFilled, trades] {cross(aggressingOrder)};
        if (fullyFilled) {
            // Fully filled so erase from order map
            orders.erase(orders.find(id));
            // Release nextID we allocated
            nextID--;
            return { id, true, trades };
        } else // Remaing qty on the order so add to a price level
        {
            assert(aggressingOrder.amount > 0);
            if (trades.size() == 0) {
                if (aggressingOrder.side == Side::bid)
                    insertIntoPriceLevel(bids, aggressingOrder);
                else
                    insertIntoPriceLevel(asks, aggressingOrder);
            } else {
                aggressingOrder.side = oppositeSide(aggressingOrder.side);
                if (aggressingOrder.side == Side::bid)
                    insertIntoPriceLevel(bids, aggressingOrder);
                else
                    insertIntoPriceLevel(asks, aggressingOrder);
            }
        }

        return { aggressingOrder.id, false, trades };
    }

    // Cancel the order and return true if cancelled
    bool cancel(OrderID id)
    {
        auto orderIter{ find(id) };
        if (orderIter == end())
            return false;
        bool result{ orderIter->second.side == Side::bid
                       ? eraseFromPriceLevel(bids, orderIter->second.price, orderIter->second.id)
                       : eraseFromPriceLevel(asks, orderIter->second.price, orderIter->second.id) };
        orders.erase(orderIter);
        return result;
    }

    // Clear out all orders
    void clear()
    {
        bids.clear();
        asks.clear();
        orders.clear();
    }

    // Get a view on the bids in price/time order
    auto getBids() const { return bids | std::views::values | std::views::join; }
    // Get a view on the asks in price/time order
    auto getAsks() const { return asks | std::views::values | std::views::join; }

private:
    using OrderQueue = std::list<Order*>;
    // We want to be able to:
    // 1. cancel in maybe log(N) or better.
    // 2. insert in log(N) or better.
    // 3. cross in a non-geometric time.

    // We need to be able to find an order by ID and we don't
    // want to search O(N) for it since we want cancel to be quick.
    using Orders = std::unordered_map<OrderID, Order>;

    // To store orders it's handy if they are stored by price level
    // so it's easy to find the inside price levels for crossing.
    // Options:
    // 1. PriorityQueue - does what we need but a debug print of the thing in order isn't possible without additional sorting.
    // 2. Map - redblack type tree, balanced, not as efficiently stored as a PriorityQueue but sorted.
    // 3. Using arrays, paying some algorithmic cost for better cache performance. A bit of an overkill for this.
    using Bids = std::map<double, OrderQueue, std::greater<double>>;
    using Asks = std::map<double, OrderQueue, std::less<double>>;

    // insert the order at the end of the price level
    void insertIntoPriceLevel(auto& ordersByPrice, Order& order)
    {
        assert(find(order.id) != end());
        auto [it, created]{ ordersByPrice.try_emplace(order.price, OrderQueue()) };
        it->second.push_back(&order);
    }

    // erase the order from the price level
    bool eraseFromPriceLevel(auto& ordersByPrice, Price price, OrderID id)
    {
        assert(ordersByPrice.contains(price));
        // Find the price level, remove the order and then remove the price level it it's empty
        OrderQueue& priceLevel{ ordersByPrice.find(price)->second };
        bool result{ std::erase_if(priceLevel, [id](const auto& value) { return value->id == id; }) > 0 };
        if (priceLevel.size() == 0)
            ordersByPrice.erase(price);

        return result;
    }

    // Tries to cross the book and generate trades.
    // Returns True if the order was fully fulled.
    std::pair<bool, Trades> cross(Order& aggressingOrder)
    {
        Trades trades;

        while (true) {
            auto&[insidePassivePrice, passiveOrders] = *(aggressingOrder.side == Side::bid ? asks.begin() : bids.begin());

            // If no orders on side break
            if (!passiveOrders.size())
                break;

            // If it won't cross then we are done
            if ((aggressingOrder.side == Side::bid && insidePassivePrice > aggressingOrder.price) || 
                (aggressingOrder.side == Side::ask && insidePassivePrice < aggressingOrder.price))
                break;

            Order* passive{ *passiveOrders.begin() };
            Qty matchAmount{ std::min(aggressingOrder.amount, passive->amount) };
            aggressingOrder.amount -= matchAmount;
            passive->amount -= matchAmount;
            // We only publish trades for the passive orders
            trades.emplace_back(passive->id, nextExecID++, passive->side, passive->price, matchAmount);
            if (passive->amount == 0) {
                cancel(passive->id);
            }
            if (aggressingOrder.amount == 0) {
                return std::pair{true, trades};
            }
        }

        return std::pair{false, trades};
    }

    // The next order-id we will assign to a new order
    OrderID nextID{ 1 };
    // The next trade-id we will assign to a trade
    ExecID nextExecID{ 1 };
    // List of bid price levels in price level order
    Bids bids;
    // List of ask price levels in price level order
    Asks asks;
    // List of orders - this manages order lifetimes
    Orders orders;
};
};
