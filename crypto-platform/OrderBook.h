#pragma once

#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <string>
#include <vector>
#include <set>

class OrderBook
{
    public:
        using KnownProducts = std::set<std::string>;
        using OrderBookEntryList = std::vector<OrderBookEntry>;

    /** construct, reading a csv data file */ 
        OrderBook(const std::string &fileName);
    /** return unordered collection of known products in the order book */ 
        KnownProducts getKnownProducts();
    /** return vector of Orders according to the sent filters */ 
        OrderBookEntryList getOrders(OrderBookType type,
                                     std::string product, 
                                     std::string timestamp);

        /** Gets the first timestamp from the CSV */ 
        std::string getEarliestTime();
        /** returns the next time after the sent time in the orderBook.
         * If there is no next timestamp,
         * wraps around back to the start. */ 
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        OrderBookEntryList matchSystem(std::string product, std::string timestamp);

        static double getHighPrice(const OrderBook::OrderBookEntryList& orders);
        static double getLowPrice(const OrderBook::OrderBookEntryList& orders);
        static double getAvgPrice(const OrderBook::OrderBookEntryList& orders);
        static void testStats();

    private:
        OrderBookEntryList orders;
};