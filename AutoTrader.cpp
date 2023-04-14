#include "AutoTrader.h"
#include "OrderBook.h"
#include "CSVReader.h"

#include <iostream>
#include <vector>
#include <map>

AutoTrader::AutoTrader()
{

}

void AutoTrader::callAutoTrader()
{
    system("clear");
    while (true)
    {
        AutoTrader::autoTradeMenu();
        int userOption;
        userOption = AutoTrader::getUserOption();
        AutoTrader::processUserOption(userOption);
        if (userOption == 6) break;
    }

}

void AutoTrader::autoTradeMenu()
{
    std::cout << "Welcome to the Automatic Trading system." << std::endl;
    std::cout << "Please select an option below:\n" << std::endl;
    std::cout << "1: Select currencies and amount:" << std::endl;
    std::cout << "2: Stop Loss" << std::endl;
    std::cout << "3: Expected Returns" << std::endl;
    std::cout << "4: Start Auto Trader" << std::endl;
    std::cout << "5: Stop Auto Trader" << std::endl;
    std::cout << "6: <--Return " << std::endl;
}

int AutoTrader::getUserOption(bool verbose)
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
    
    if (verbose == true)
    {
        std::cout << "\n    Option: " << userOption << " Selected.\n" << std::endl;
    }
    return userOption;
}

void AutoTrader::processUserOption(int userOption)
{
    if (userOption == 0)
    {
        AutoTrader::invalidChoice();
    }
    if (userOption == 1)
    {
        AutoTrader::currencySelection();
    }
    if (userOption == 2)
    {
        AutoTrader::stopLoss();
    }
    if (userOption == 3)
    {
        AutoTrader::ROI();
    }
    if (userOption == 4)
    {
        AutoTrader::autoStart();
    }
    if (userOption == 5)
    {
        AutoTrader::autoStop();
    }
    if (userOption > 6)
    {
        AutoTrader::invalidChoice();
    }
}

void AutoTrader::invalidChoice()
{
    std::cout << "Invalid Choice. Please seletct a number from 1-6." << std::endl;
}

void AutoTrader::currencySelection()
{
    std::map<std::string, double> currMap;
    std::cout << "\nAvailable currencies, enter your amount or '/' to skip:"  << std::endl;

    for (std::string const& p: orderBook.getKnownProducts())
    {
        std::cout << "-" << p << ": ";
        std::string string;
        std::getline(std::cin, string);
        if (string == "/") continue;
        else
        {
            currMap.insert({p, std::stod(string)});
        }

    }

    std::cout << "\nCurrencies successfully entered." << std::endl;

    // while (true)
    // {

    //     std::string string;
    //     std::getline(std::cin, string);
    //     if (string.length() < 2)
    //     {
    //         std::cout << "Invalid input.\n" << std::endl;
    //         break;
    //     }

    //     std::vector<std::string> tokens = CSVReader::tokenise(string, ',');
    //     if (std::stoi(tokens[1]))
    //     {
    //         currMap.insert({tokens[0], std::stod(tokens[1])});
    //     }
        
    // }

    for(const auto& key_value: currMap) {
        std::string key = key_value.first;
        int value = key_value.second;
        
        std::cout << key << " - " << value << std::endl;
    }
}

int AutoTrader::stopLoss()
{
    std::cout << "Please enter your prefered % stop loss: ";
    stopLossOption = AutoTrader::getUserOption(false);
    return 0;
}

int AutoTrader::ROI()
{
    std::cout << "Please enter your prefered % ROI: ";
    roi = AutoTrader::getUserOption(false);
    return 0;
}

void AutoTrader::autoStart()
{
    /** Loop through currMap and create a orderbook entry ask/bid
     * Keep in mind the stop loss and the ROI
     * Once stop loss reached, exit
     * Once ROI reaches, exit
     * Return the wallet amount at the end along with number of trades made
     * (May need a separte cpp for stratergies etc)*/ 
    
}

void AutoTrader::autoStop()
{

}

bool AutoTrader::autoDetermineEnd()
{
    return false;
}