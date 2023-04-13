#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "Wallet.h"
#include "AutoTrader.h"
#include <iostream>
#include <vector>
#include <string>

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int userOption;
    currentTime = orderBook.getEarliestTime();
    wallet.insertCurrency("BTC", 10);
    while (true)
    {
        printMenu();
        userOption = getUserOption();
        processUserOption(userOption);
    }
}

void MerkelMain::printMenu()
{
    std::cout << "\n==================" << std::endl;
    std::cout << "1: Print Help" << std::endl; 
    std::cout << "2: Print Exchange Stats" << std::endl; 
    std::cout << "3: Make A Ask" << std::endl; 
    std::cout << "4: Make A Bid" << std::endl; 
    std::cout << "5: Wallet State" << std::endl; 
    std::cout << "6: Continue" << std::endl; 
    std::cout << "7: AutoTrader Options" << std::endl; 
    std::cout << "==================" << std::endl;
    std::cout << "Select a option from 1-6" << std::endl; 
    std::cout << "Current time: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - Your aim is to make money." << std::endl;
}

void MerkelMain::exchangeStats()
{
    std::cout << "Exchange Stats:\n" << std::endl;
    printOrderBookStats();
}

void MerkelMain::makeAsk()
{
    std::cout << "Make A Ask - Enter the amout: *product, price, amount* e.g. ETH/BTC,1,0.5" << std::endl;
    std::string askInput;
    std::getline(std::cin, askInput);
    std::vector<std::string> tokens = CSVReader::tokenise(askInput, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Invalid input, please refer to the example e.g. ETH/BTC,1,0.5" << std::endl;
    }
    else
    {
        try{
            OrderBookEntry OBE = CSVReader::stringToOBE(currentTime, tokens[0], OrderBookType::ask, tokens[1], tokens[2]);
            OBE.username = "simuser";
            if (wallet.fulfillOrder(OBE))
            {
                std::cout << "Wallet can fullful order." << std::endl;
            }
            else
            {
                std::cout << "==Insuficient funds==" << std::endl;
            }
            orderBook.insertOrder(OBE);
        }catch(const std::exception& e)
        {
            std::cout << "MerkelMain::makeAsk Invalid input."<< std::endl;
        }
    }
}

void MerkelMain::makeBid()
{
    std::cout << "Make A Bid - Enter the amount: *product, price, amount* e.g. ETH/BTC,1,0.5" << std::endl;
    std::string askInput;
    std::getline(std::cin, askInput);
    std::vector<std::string> tokens = CSVReader::tokenise(askInput, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Invalid input, please refer to the example e.g. ETH/BTC,1,0.5" << std::endl;
    }
    else
    {
        try{
            OrderBookEntry OBE = CSVReader::stringToOBE(currentTime, tokens[0], OrderBookType::bid, tokens[1], tokens[2]);
            OBE.username = "simuser";
            if (wallet.fulfillOrder(OBE))
            {
                std::cout << "Wallet can fullful order." << std::endl;
            }
            else
            {
                std::cout << "==Insuficient funds==" << std::endl;
            }
            orderBook.insertOrder(OBE);
        }catch(const std::exception& e)
        {
            std::cout << "MerkelMain::makeBid Invalid input."<< std::endl;
        }
    }
}

void MerkelMain::walletState()
{
    if (wallet.toString().length() == 0)
    {
        std::cout << "Your wallet is empty." << std::endl;
    }
    else
    {
        std::cout << wallet.toString();
    }
    
}

void MerkelMain::processTrade()
{
    std::cout << "Processing trade..." << std::endl;
    std::vector<OrderBookEntry> sales = orderBook.matchSystem("ETH/BTC", currentTime);
    std::cout << "No. Sales: " << sales.size() << std::endl;
    for (OrderBookEntry& sale: sales)
    {
        std::cout << "Sale price: " << sale.price << " | Sale amount: " << sale.amount << std::endl;
        if (sale.username == "simuser")
        {
            wallet.processSale(sale);
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::invalidChoice()
{
    std::cout << "Invalid Choice. Please seletct a number from 1-6." << std::endl;
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    
    std::cout << "\nOption: " << userOption << " Selected." << std::endl;

    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        MerkelMain::invalidChoice();
    }

    if (userOption == 1)
    {
        MerkelMain::printHelp();
    }

    if (userOption == 2)
    {
        MerkelMain::exchangeStats();
    }

    if (userOption == 3)
    {
        MerkelMain::makeAsk();
    }

    if (userOption == 4)
    {
        MerkelMain::makeBid();
    }

    if (userOption == 5)
    {
        MerkelMain::walletState();
    }

    if (userOption == 6)
    {
        MerkelMain::processTrade();
    }

    if (userOption == 7)
    {
        autoTrade.callAutoTrader();
    }

    if (userOption > 7) // bad input
    {
        MerkelMain::invalidChoice();
    }
}

void MerkelMain::printOrderBookStats()
{
    OrderBook::testStats();
    std::cout << "Available products:" << std::endl;
    for (std::string const& p: orderBook.getKnownProducts())
    {
        std::cout << "- " << p;
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask,
                                                                  p,
                                                                  currentTime);
        std::cout << " - Asks seen: " << askEntries.size();
        std::cout << " | Max Ask: " << OrderBook::getHighPrice(askEntries);
        std::cout << " | Min Ask: " << OrderBook::getLowPrice(askEntries);
        std::cout << " | Spread " << OrderBook::getHighPrice(askEntries) - OrderBook::getLowPrice(askEntries);
        std::cout << " | Avg Price: " << OrderBook::getAvgPrice(askEntries) << std::endl;

        std::cout << "- " << p;
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid,
                                                                  p,
                                                                  currentTime);
        std::cout << " - Bids seen: " << bidEntries.size();
        std::cout << " | Max Bid: " << OrderBook::getHighPrice(bidEntries);
        std::cout << " | Min Bid: " << OrderBook::getLowPrice(bidEntries);
        std::cout << " | Spread " << OrderBook::getHighPrice(bidEntries) - OrderBook::getLowPrice(bidEntries);
        std::cout << " | Avg Price: " << OrderBook::getAvgPrice(bidEntries) << "\n" << std::endl;
    }
}