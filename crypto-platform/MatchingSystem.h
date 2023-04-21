#include <vector>
#include <iostream>
#include <crypto-platform/OrderBookEntry.h>


class MatchSystem{

    public:
        MatchSystem();
        void init();
        OrderBookEntry csvStringToOBE(std::vector<std::string> tokens);
        std::vector<std::string> tokenise(std::string csvLine, char separator);
        int readCSV_NEW(std::string fileName);
        std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matchEngine(OrderBookEntry bid_order);
        std::vector<std::pair<OrderBookEntry, OrderBookEntry>> testMatch(bool size);

    private:
        std::map<std::string, std::map<std::string,
        std::map<std::string, std::vector<OrderBookEntry>>>> allOrders;
};

class Trades{

    public:
        Trades(std::string _timestamp,
                std::string _buyCurrency,
                OrderBookType _sellCurrency,
                double _buyPrice,
                double _sellPrice);

        std::string timestamp;
        std::string buyCurrency;
        OrderBookType sellCurrency;
        double buyPrice;
        double sellPrice;
                

};