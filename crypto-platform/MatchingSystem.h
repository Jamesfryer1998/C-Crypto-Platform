#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/TradeBookEntry.h>

class MatchSystem{

    public:
        MatchSystem();

        // OrderBook Initialisation
        void init();
        void clearOrderBook();
        int readCSV_NEW(std::string fileName);
        OrderBookEntry csvStringToOBE(std::vector<std::string> tokens);
        std::vector<std::string> tokenise(std::string csvLine, char separator);

        // OrderBook Operations
        void removeOrder(std::string product, std::string type);
        void inserOrder(std::string product, std::string type, OrderBookEntry order);
        std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matchEngine(OrderBookEntry bid_order);

        // Trade Operations
        void addTrade(std::string _timestamp,std::string _buyCurrency,std::string _sellCurrency,double _buyPrice,
                                                                                                double _sellPrice);
        double totalBuy(std::vector<TradeBookEntry> trades);
        double totalSell(std::vector<TradeBookEntry> trades);

        // Getters
        auto getOrderBook()
        {
            return orderBook;
        }

        auto getTrades()
        {
            return trades;
        }

    private:
        std::vector<TradeBookEntry> trades;
        std::map<std::string, std::map<std::string,
        std::map<std::string, std::vector<OrderBookEntry>>>> orderBook;
};