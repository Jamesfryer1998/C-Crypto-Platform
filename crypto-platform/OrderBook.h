#pragma once

#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <string>
#include <vector>

class OrderBook
{
    public:
        OrderBook(std::string fileName);

        // OrderBook Operations
        void insertOrder(OrderBookEntry& order);
        void removeOrder(OrderBookEntry& order);
        std::vector<std::string> getKnownProducts();
        std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                              std::string product, 
                                              std::string timestamp);
        std::vector<OrderBookEntry> matchSystem(std::string product, std::string timestamp);


        // THIS NEEDS TO GO!!!!!!!!!!!!!!!!!!!!!!!
        std::vector<OrderBookEntry> getOrdersOfCurrency(OrderBookType type,
                                              std::string product, 
                                              std::string timestamp);

        // Timestamp Operations
        std::string getEarliestTime();
        std::string getNextTime(std::string timestamp);

        // Statistical OrderBook Operations
        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        static double getAvgPrice(std::vector<OrderBookEntry>& orders);
        static void testStats();

    private:
        std::vector<OrderBookEntry> orders;
};