#pragma once

#include <vector>
#include <iostream>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/TradeBookEntry.h>

class MatchSystem{

    public:
        MatchSystem();
        void init();
        void clearOrderBook();
        int readCSV_NEW(std::string fileName);
        double totalBuy(std::vector<TradeBookEntry> trades);
        double totalSell(std::vector<TradeBookEntry> trades);
        void removeOrder(std::string product, std::string type);
        OrderBookEntry csvStringToOBE(std::vector<std::string> tokens);
        std::vector<std::string> tokenise(std::string csvLine, char separator);
        void inserOrder(std::string product, std::string type, OrderBookEntry order);
        std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matchEngine(OrderBookEntry bid_order);
        void addTrade(std::string _timestamp,std::string _buyCurrency,std::string _sellCurrency,double _buyPrice,
                                                                                                double _sellPrice);
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