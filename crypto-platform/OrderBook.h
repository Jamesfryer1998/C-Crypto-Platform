#pragma once

#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <string>
#include <vector>
#include <set>

class OrderBook
{
    public:
    /** construct, reading a csv data file */ 
        OrderBook(std::string fileName);
    /** return vector of all knwon products in dataset */ 
        std::set<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters */ 
        std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                              std::string product, 
                                              std::string timestamp);

        /** Gets the first timestamp from the CSV */ 
        std::string getEarliestTime();
        /** returns the next time after the sent time in the orderBook.
         * If there is no next timestamp,
         * wraps around back to the start. */ 
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchSystem(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        static double getAvgPrice(std::vector<OrderBookEntry>& orders);
        static void testStats();

    private:
        std::vector<OrderBookEntry> orders;
};