#include <crypto-platform/AutoTrader.h>
#include <crypto-platform/OrderBook.h>
#include <crypto-platform/CSVReader.h>

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
        userOption = getUserOption();
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
        AutoTrader::currencySelectionNew();
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

void AutoTrader::currencySelectionNew()
{
    // 1. Promot the user to enter a currency and amount
    // 2. Check if this currency is in wallet (tokenise their answer, and use tokens[0])
    // 3. If TRUE when add the currency to the list
    // 4. If false then call invalidChoice() and ask to try again

    std::cout << "Please enter a currency you wish to trade with: (USDT,1000) '/'  to exit" << std::endl;

    while (true)
    {
        std::string currencyInput;
        std::getline(std::cin, currencyInput);
        std::vector<std::string> tokens = CSVReader::tokenise(currencyInput, ',');

        if (currencyInput == "/") break;
        if (tokens.size()!= 2) std::cout << "Invalid Input." << std::endl;
        else
        {
            std::cout << tokens[0] << std::endl;
            double currencyAmount = std::stod(tokens[1]);
            bool walletContains = wallet.containsCurrency(tokens[0], currencyAmount);

            if (walletContains == false)
            {
                std::cout << "Insuficient funds for " << tokens[0] << " in wallet." << std::endl;
            }
            else
            {
                if(currMap.count(tokens[0])) continue;
                else currMap.insert({tokens[0], currencyAmount});
            }
        }
    }
}

void AutoTrader::currencySelection()
{
    std::cout << "\nAvailable currencies, enter your amount or '/' to skip:"  << std::endl;

    for (std::string const& p: orderBook.getKnownProducts())
    {
        std::cout << "-" << p << ": ";
        std::string string;
        std::getline(std::cin, string);
        if (string == "/") continue;
        else
        {
            if(currMap.count(p)) continue;
            else currMap.insert({p, std::stod(string)});
        }
    }


    for(const auto& key_value: currMap) {
        std::string key = key_value.first;
        int value = key_value.second;
        
        std::cout << key << " - " << value << std::endl;
    }

    std::cout << "\nCurrencies successfully entered." << std::endl;
}

int AutoTrader::stopLoss()
{
    /** Calculate final wallet amount from stop loss perfentage */ 
    std::cout << "Please enter your prefered % stop loss: ";
    stopLossOption = getUserOption(false);

    std::map<std::string, double> lossMap;

    // std::map<std::string, double> currMap = currencySelection();

    for(const auto& key_value: currMap) {
        std::string product = key_value.first;

        // Check if currency is in wallet first

        // Insert product and final wallet value into map
        // currMap.insert({product, calculated wallet value);

        
        std::cout << product << std::endl;
    }

    return 0;
}

int AutoTrader::ROI()
{
    /** Calculate final wallet about from ROI percentage */ 
    std::cout << "Please enter your prefered % ROI: ";
    roi = getUserOption(false);
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
    std::cout << "Start activate" << std::endl;
    isAutoTraderRunning = true;
    while (isAutoTraderRunning)
    {
        AutoTrader::autoStop();
        // Code that runs the auto trader program
    }
    std::cout << "Start stopped" << std::endl;
}

void AutoTrader::autoStop()
{
    isAutoTraderRunning = false;
}

bool AutoTrader::determineEnd()
{
    /** When suitable end found
     * - either stop loss reached
     * - or, ROI achieved*/ 
    return false;
}