#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>



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
    std::vector<OrderBookEntry> orders;
    OrderBookEntry entry1("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1);
    orders.push_back(entry1);
    // MatchSystem::matchEngine(orders[0]);
    MatchSystem::priceTime(orders[0]);

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

int MatchSystem::readCSV_NEW(std::string fileName)
{
    int loadCount = 0;
    std::string line;
    std::ifstream csvFile{fileName};
    std::vector<std::string> tokens;

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
                    loadCount++;
                }
                // bids
                else
                {
                    allOrders[tokens[1]]["orderType"]["bid"].push_back(OBE);
                    loadCount++;
                }
            }catch(const std::exception& e)
            {

            }
        }
    }

    // Sorting vectors
    std::string key;
    for (auto& keyValue: allOrders)
    {
        key = keyValue.first;
        std::cout << key << std::endl;

        // Sort asks
        std::sort(allOrders[key]["orderType"]["ask"].begin(),
        allOrders[key]["orderType"]["ask"].end(),
        OrderBookEntry::comapareByPriceAsc);

        // Sort bids
        std::sort(allOrders[key]["orderType"]["bid"].begin(),
        allOrders[key]["orderType"]["bid"].end(),
        OrderBookEntry::comapareByPriceDesc);
    }
    std::cout << "MatchSystem::readCSV read" << loadCount << "files" << std::endl;

    return loadCount;
} 


// Function to match a single bid order with an ask order using price-time priority
std::vector<std::pair<OrderBookEntry, OrderBookEntry>> MatchSystem::matchEngine(OrderBookEntry order)
{
    double price = order.price;
    double amount = order.amount;
    OrderBookType type = order.type;
    std::string product = order.product;
    std::string timestamp = order.timestamp;

    std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matched_orders;

    // If order is a bid, check the asks
    if (type == OrderBookType::bid)
    {
        if (allOrders[product]["orderType"]["ask"].size() > 0)
        {
            std::vector<OrderBookEntry>& askOrders = allOrders[product]["orderType"]["ask"];

            for (unsigned long i = 0; i <  askOrders.size(); i++)
            {
                OrderBookEntry& singleAsk = askOrders[i];
                if (singleAsk.price <= price)
                {
                    double matchAmount = std::min(amount, singleAsk.amount);                    
                    matched_orders.push_back(std::make_pair(order, singleAsk));                    
                    amount -= matchAmount;
                    singleAsk.amount -= matchAmount;
                    if (singleAsk.amount == 0)
                    {
                        askOrders.erase(askOrders.begin() + i);
                        i--;
                    }
                    if (amount <= 0) break;
                }
            }
        }
    }

    // If order is a ask, check the bids
    if (type == OrderBookType::ask)
    {
        if (allOrders[product]["orderType"]["bid"].size() > 0)
        {
            std::vector<OrderBookEntry>& bidOrders = allOrders[product]["orderType"]["bid"];

            for (unsigned long i = 0; i < bidOrders.size(); i++)
            {
                OrderBookEntry& singleBid = bidOrders[i];
                if (singleBid.price >= price)
                {
                    double matchAmount = std::min(amount, singleBid.amount);                    
                    matched_orders.push_back(std::make_pair(order, singleBid));
                    amount -= matchAmount;
                    singleBid.amount -= matchAmount;
                    if (singleBid.amount == 0)
                    {
                        bidOrders.erase(bidOrders.begin() + i);
                        i--;
                    }
                    if (amount <= 0) break;
                }
            }
        }
    }
    std::cout << "Matched orders number: " << matched_orders.size() << std::endl; 

    // TODO: - Add the fullfilled ordered to a trades vector
    // Find a better way to match, we are still looping through the vectors

    return matched_orders;
}

// TODO:
// 1. Add insert and remove order functions 

/////////////// TEST ///////////////////

std::vector<std::pair<OrderBookEntry, OrderBookEntry>> MatchSystem::priceTime(OrderBookEntry order)
{
    // double price = order.price;
    // double amount = order.amount;
    OrderBookType type = order.type;
    std::string product = order.product;
    std::string timestamp = order.timestamp;

    std::vector<std::pair<OrderBookEntry, OrderBookEntry>> matched_orders;

    // If order is a bid, check the asks
    if (type == OrderBookType::bid)
    {
        if (allOrders[product]["orderType"]["ask"].size() > 0)
        {
            std::vector<OrderBookEntry>& askOrders = allOrders[product]["orderType"]["ask"];

            std::sort(askOrders.begin(),askOrders.end(), OrderBookEntry::comparePriceTimestamp);
            std::cout << askOrders[0].timestamp << std::endl;
            std::cout << askOrders.back().timestamp << std::endl;

            // Find the minimum price using std::min_element and a lambda function
            auto minPriceEntry = std::min_element(askOrders.begin(), askOrders.end(), [](const OrderBookEntry& a, const OrderBookEntry& b) {
                return a.timestamp < b.timestamp;
            });

            std::cout << "Minimum price: " << minPriceEntry->timestamp << std::endl;
        }
    }       
    return matched_orders;
}