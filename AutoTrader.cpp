#include "AutoTrader.h"
#include <iostream>

AutoTrader::AutoTrader()
{

}

void AutoTrader::callAutoTrader()
{
    AutoTrader::autoTradeMenu();
    int userOption;
    userOption = AutoTrader::getUserOption();
}

void AutoTrader::autoTradeMenu()
{
    std::cout << "Welcome to the Automatic Trading system." << std::endl;
    std::cout << "Please follow the instructions below:" << std::endl;
    std::cout << "1: Select currencies and amount" << std::endl;
    std::cout << "2: Stop Loss" << std::endl;
    std::cout << "3: Expected Returns" << std::endl;
    std::cout << "4: Start Auto Trader" << std::endl;
    std::cout << "5: Stop Auto Trader" << std::endl;
}

int AutoTrader::getUserOption()
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

void AutoTrader::autoStart()
{

}

void AutoTrader::autoStop()
{

}

bool AutoTrader::autoDetermineEnd()
{
    return false;
}