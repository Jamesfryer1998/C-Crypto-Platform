#include <vector>
#include <iostream>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/TradeBookEntry.h>

class MatchSystem{

    public:
        MatchSystem();
        void init();
        OrderBookEntry csvStringToOBE(std::vector<std::string> tokens);
        std::vector<std::string> tokenise(std::string csvLine, char separator);
        int readCSV_NEW(std::string fileName);
        std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matchEngine(OrderBookEntry bid_order);

        void clearOrderBook();
        void inserOrder(std::string product, std::string type, OrderBookEntry order);
        void removeOrder(std::string product, std::string type);
        void addTrade(std::string _timestamp,
                                    std::string _buyCurrency,
                                    std::string _sellCurrency,
                                    double _buyPrice,
                                    double _sellPrice);

        double totalBuy(std::vector<TradeBookEntry> trades);
        double totalSell(std::vector<TradeBookEntry> trades);
    

        // This can go after insert and remove functions added
        auto getOrderBook()
        {
            return orderBook;
        }

        auto getTrades()
        {
            return trades;
        }

    private:
        std::map<std::string, std::map<std::string,
        std::map<std::string, std::vector<OrderBookEntry>>>> orderBook;
        std::vector<TradeBookEntry> trades;
};