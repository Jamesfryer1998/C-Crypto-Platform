#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

#include <crypto-platform/MatchingSystem.h>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <crypto-platform/TradeBookEntry.h>

MatchSystem::MatchSystem()
{
    
}

void MatchSystem::init()
{
    MatchSystem::readCSV_NEW("20200317.csv");
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
                    orderBook[tokens[1]]["orderType"]["ask"].push_back(OBE);
                    loadCount++;
                }
                // bids
                else
                {
                    orderBook[tokens[1]]["orderType"]["bid"].push_back(OBE);
                    loadCount++;
                }
            }catch(const std::exception& e)
            {

            }
        }
    }

    // Sorting vectors
    std::string key;
    for (auto& keyValue: orderBook)
    {
        key = keyValue.first;
        // std::cout << key << " loaded." << std::endl;

        // Sort asks
        std::sort(orderBook[key]["orderType"]["ask"].begin(),
        orderBook[key]["orderType"]["ask"].end(),
        OrderBookEntry::comapareByPriceAsc);

        // Sort bids
        std::sort(orderBook[key]["orderType"]["bid"].begin(),
        orderBook[key]["orderType"]["bid"].end(),
        OrderBookEntry::comapareByPriceDesc);
    }
    std::cout << loadCount << " orders loaded." << std::endl;

    return loadCount;
} 

std::vector<std::string> MatchSystem::getProductsOfCurrency(std::string currency)
{
    std::vector<std::string> currVec;
    for (auto& product: orderBook)
    {
        std::vector<std::string> tokens = tokenise(product.first, '/');
        if (currency == tokens[0] || currency == tokens[1])
        {
            currVec.push_back(product.first); 
        }
    }
    return currVec;
}

void MatchSystem::clearOrderBook()
{
    orderBook.clear();
}

void MatchSystem::inserOrder(std::string product, std::string type, OrderBookEntry order)
{
    orderBook[product]["orderType"][type].push_back(order);
}

void MatchSystem::removeOrder(std::string product, std::string type)
{
    orderBook[product]["orderType"][type].erase(orderBook[product]["orderType"][type].end() - 1);
}

void MatchSystem::addTrade(std::string timestamp,
                            std::string buyCurrency,
                            std::string sellCurrency,
                            double buyPrice,
                            double sellPrice)
{
    TradeBookEntry trade(timestamp, buyCurrency, sellCurrency, buyPrice, sellPrice);
    trades.push_back(trade);
}

double MatchSystem::totalBuy(std::vector<TradeBookEntry> trades)
{
    double sum_of_prices = std::accumulate(trades.begin(), trades.end(), 0.0,
    [](double prices, const TradeBookEntry& trade) { return prices + trade.buyPrice; });
    return sum_of_prices;
}

double MatchSystem::totalSell(std::vector<TradeBookEntry> trades) 
{
    double sum_of_prices = std::accumulate(trades.begin(), trades.end(), 0.0,
    [](double prices, const TradeBookEntry& trade) { return prices + trade.sellPrice; });
    return sum_of_prices;
}

// TODO:
// - More Trade book stats, maybe for each product

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
        if (orderBook[product]["orderType"]["ask"].size() > 0)
        {
            std::vector<OrderBookEntry>& askOrders = orderBook[product]["orderType"]["ask"];

            for (unsigned long i = 0; i <  askOrders.size(); i++)
            {
                OrderBookEntry& singleAsk = askOrders[i];
                if (singleAsk.price <= price)
                {
                    double matchAmount = std::min(amount, singleAsk.amount);                    
                    matched_orders.push_back(std::make_pair(order, singleAsk));                    
                    amount -= matchAmount;
                    singleAsk.amount -= matchAmount;

                    // Adding a trade
                    std::vector<std::string> currencies = CSVReader::tokenise(product, '/');
                    std::cout << timestamp << currencies[0] << currencies[1] << price << matchAmount << std::endl;
                    MatchSystem::addTrade(timestamp, currencies[0], currencies[1], price, matchAmount);
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
        if (orderBook[product]["orderType"]["bid"].size() > 0)
        {
            std::vector<OrderBookEntry>& bidOrders = orderBook[product]["orderType"]["bid"];

            for (unsigned long i = 0; i < bidOrders.size(); i++)
            {
                OrderBookEntry& singleBid = bidOrders[i];
                if (singleBid.price >= price)
                {
                    double matchAmount = std::min(amount, singleBid.amount);                    
                    matched_orders.push_back(std::make_pair(order, singleBid));
                    // Sort bidOrders
                    amount -= matchAmount;
                    singleBid.amount -= matchAmount;

                    // Adding a tade
                    std::vector<std::string> currencies = CSVReader::tokenise(product, '/');
                    std::cout << timestamp << currencies[0] << currencies[1] << price << matchAmount << std::endl;
                    MatchSystem::addTrade(timestamp, currencies[0], currencies[1], price, matchAmount);
                    
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
    
    // TODO:
    // Find a better way to match, we are still looping through the vectors

    return matched_orders;
}

