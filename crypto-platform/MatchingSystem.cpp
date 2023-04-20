#include <iostream>
#include <vector>
#include <fstream>
#include <map>

#include <crypto-platform/MatchingSystem.h>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>

MatchSystem::MatchSystem()
{
    
}

Trades::Trades( std::string _timestamp,
                std::string _buyCurrency,
                OrderBookType _sellCurrency,
                double _buyPrice,
                double _sellPrice)
                
: timestamp(_timestamp),
  buyCurrency(_buyCurrency),
  sellCurrency(_sellCurrency),
  buyPrice(_buyPrice),
  sellPrice(_sellPrice)
{
    
}

void MatchSystem::init()
{
    MatchSystem::readCSV_NEW("20200317.csv");
    // std::vector<OrderBookEntry> asks = MatchSystem::createAsks();
    // std::vector<OrderBookEntry> bids = MatchSystem::createBids()

    // FIXME: Why is this not working?
    MatchSystem::matchEngine(MatchSystem::createAsks(), MatchSystem::createBids());
}

OrderBookEntry MatchSystem::csvStringToOBE(std::vector<std::string> tokens)
{
    double price, amount; 

    if (tokens.size() != 5) 
    {
        throw std::exception{};
    }
    try{
        std::string timestamp = tokens[0];
        std::string product = tokens[1];
        std::string orderType = tokens[2];
        price = std::stod(tokens[3]);
        amount = std::stod(tokens[4]);
    }catch(const std::exception& e)
    {
        std::cout << "Bad data." << std::endl;
        throw;
    }
    OrderBookEntry OBE{tokens[0], tokens[1], OrderBookEntry::StringToOrderBookType(tokens[2]), price, amount};
    return OBE;
}

std::vector<std::string> MatchSystem::tokenise(std::string csvLine, char separator)
{
    std::vector<std::string> tokens;
    
    std::string::size_type start, end;
    std::string token;
    start = csvLine.find_first_not_of(separator, 0);

    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break;
        if (end >= 0 ) token = csvLine.substr(start, end - start);
        else token = csvLine.substr(start, csvLine.length() - start);
        tokens.push_back(token);
        start = end + 1;
    }while(end != std::string::npos);

    return tokens;
}

void MatchSystem::readCSV_NEW(std::string fileName)
{
    std::ifstream csvFile{fileName};
    std::string line;
    std::vector<std::string> tokens;

    std::vector<std::string> products;
    std::map<std::string, bool> prodMap;

    if (csvFile.is_open())
    {
        while (std::getline(csvFile, line))
        {

            try{
                tokens = tokenise(line, ',');
                MatchSystem match;
                OrderBookEntry OBE = match.csvStringToOBE(tokens);

                // asks
                if (tokens[2] == "ask")
                {
                    allOrders[tokens[1]]["orderType"]["ask"].push_back(OBE);
                }
                // bids
                else
                {
                    allOrders[tokens[1]]["orderType"]["bid"].push_back(OBE);
                }
            }catch(const std::exception& e)
            {

            }
        }
    }

    std::string key;
    for (auto& keyValue: allOrders)
    {
        key = keyValue.first;
        std::cout << key << std::endl;

        // Sort asks
        std::sort(allOrders[key]["orderType"]["ask"].begin(),
        allOrders[key]["orderType"]["ask"].end(),
        OrderBookEntry::comapareByPriceAsc);

        // Sort asks
        std::sort(allOrders[key]["orderType"]["bid"].begin(),
        allOrders[key]["orderType"]["bid"].end(),
        OrderBookEntry::comapareByPriceAsc);
    }
    std::cout << "MatchSystem::readCSV read" << allOrders["ETH/BTC"]["orderType"]["ask"].size() << "files" << std::endl;
} 

std::vector<OrderBookEntry> MatchSystem::createBids()
{
    std::vector<OrderBookEntry> bids;
    bids.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "BTC/ETH", OrderBookType::bid, 
                                  7.44564869, 0.02187308));
    bids.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "BTC/ETH", OrderBookType::bid, 
                                  7.44564869, 0.03187308));
    bids.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "BTC/ETH", OrderBookType::bid, 
                                  7.44564869, 0.04187308));
    bids.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "BTC/ETH", OrderBookType::bid, 
                                  7.44564869, 0.05187308));
    bids.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "BTC/ETH", OrderBookType::bid, 
                                  7.44564869, 0.06187308));
    return bids;
}

std::vector<OrderBookEntry> MatchSystem::createAsks()
{
    std::vector<OrderBookEntry> asks;
    asks.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "ETH/BTC", OrderBookType::ask, 
                                  0.02187308,7.44564869));
    asks.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "ETH/BTC", OrderBookType::ask, 
                                  0.03187308,7.44564869));
    asks.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "ETH/BTC", OrderBookType::ask, 
                                  0.04187308,7.44564869));
    asks.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "ETH/BTC", OrderBookType::ask, 
                                  0.05187308,7.44564869));
    asks.push_back(OrderBookEntry("2020/03/17 17:01:24.884492",
                                  "ETH/BTC", OrderBookType::ask, 
                                  0.06187308,7.44564869));
    return asks;
}

void MatchSystem::matchEngine(std::vector<OrderBookEntry>& bids,
                              std::vector<OrderBookEntry>& asks)
{
    std::string key;
    for (auto& bid: bids)
    {
        key = bid.product;

        std::vector<OrderBookEntry> allOrderAsks = allOrders[key]["orderType"]["ask"];
        std::cout << allOrderAsks[0].product << std::endl;

    }
}