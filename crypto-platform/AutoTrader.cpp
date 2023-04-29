#include <crypto-platform/AutoTrader.h>
#include <crypto-platform/OrderBook.h>
#include <crypto-platform/CSVReader.h>

#include <iostream>
#include <vector>
#include <map>

AutoTrader::AutoTrader(OrderBook _orderbook)
: orderBook(_orderbook)
{

}

void AutoTrader::callAutoTrader(Wallet walletInput)
{
    system("clear");
    std::cout << "===== USING MATCHING SYSTEM =====\n" << std::endl;
    match.init();
    wallet = walletInput;
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
    std::cout << "\nWelcome to the Automatic Trading system." << std::endl;
    std::cout << "Please select an option below:\n" << std::endl;
    std::cout << "1: Select currencies and amount:" << std::endl;
    std::cout << "2: Stop Loss %" << std::endl;
    std::cout << "3: Expected Returns (ROI) %" << std::endl;
    std::cout << "4: Start Auto Trader" << std::endl;
    std::cout << "5: Stop Auto Trader" << std::endl;
    std::cout << "6: <--Return (legacy system)" << std::endl;
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
        // Do nothing
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

/** 1. Promot the user to enter a currency and amount
 * 2. Check if this currency is in wallet (tokenise their answer, and use tokens[0])
 * 3. If TRUE when add the currency to the list
 * 4. If false then call invalidChoice() and ask to try again */ 
void AutoTrader::currencySelection()
{
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
            double currencyAmount = std::stod(tokens[1]);
            bool walletContains = wallet.containsCurrency(tokens[0], currencyAmount);

            if (walletContains == 0)
            {
                std::cout << "Insuficient amount of " << tokens[0] << " in wallet." << std::endl;
            }
            else
            {
                if(currMap.count(tokens[0])) continue;
                else currMap.insert({tokens[0], currencyAmount});
            }
        }
    }
    tradeCurrMap = currMap;
}

/** Calculate final wallet amount from stop loss perfentage */ 
void AutoTrader::stopLoss()
{
    // TODO:
    std::cout << "Please enter your prefered % stop loss: ";
    stopLossOption = getUserOption(false);

    for(const auto& key_value: currMap) {
        std::string product = key_value.first;
        double finalAmount = key_value.second * (100 - stopLossOption)/100;
        lossMap.insert({product, finalAmount});
        std::cout << product << " : " << finalAmount << std::endl;
    }
}

/** Calculate final wallet about from ROI percentage */ 
void AutoTrader::ROI()
{
    // TODO:
    std::cout << "Please enter your prefered % ROI: ";
    roi = getUserOption(false);

    for(const auto& key_value: currMap) {
        std::string product = key_value.first;
        double finalAmount = key_value.second * (100 + stopLossOption)/100;
        roiMap.insert({product, finalAmount});
        std::cout << product << " : " << finalAmount << std::endl;
    }
}








































void AutoTrader::autoAsk(std::string product, std::string timestamp)
{

    // FIXME: Segmentation error?? Not sure why.

    /** Makes automatic asks in orderbook
     * 1. Gets all asks from order book
     * 2. Finds the cheapest asks for each product at current time point
     * 3. Creates orderbookentry and inserts order into orderbook
     *      - If ROI not reached
     *      - If stop loss not reached */ 

    std::vector<OrderBookEntry> askEntries = orderBook.getOrdersOfCurrency(OrderBookType::ask,
                                                                product, timestamp);

    double amountToTrade = currMap[product] / numberTrades;
    double lowestPrice = orderBook.getLowPrice(askEntries);
    std::cout << lowestPrice;

    for (OrderBookEntry& asks: askEntries)
    {
        if (asks.price == lowestPrice && 
        tradeCurrMap[product] < lossMap[product] &&
        tradeCurrMap[product] > roiMap[product])
        {
            OrderBookEntry OBE = CSVReader::stringToOBE(timestamp, asks.product, OrderBookType::ask, std::to_string(lowestPrice), std::to_string(amountToTrade));
            orderBook.insertOrder(OBE);

            tradeCurrMap[product] -= amountToTrade;
            tradeCount ++;

            // TODO: 
            // 1. Will need to remove order from order book once trade processed (OrderBook::removeOrder())
            // 2. Duduct the traded amount from the currMap
            // 3. Count the trade in a variable
        }
    }
}

// void AutoTrader::autoAsk()
// {
//     std::cout << "Make A Ask - Enter the amout: *product, price, amount* e.g. ETH/BTC,1,0.5" << std::endl;
//     std::string askInput;
//     std::getline(std::cin, askInput);
//     std::vector<std::string> tokens = CSVReader::tokenise(askInput, ',');
//     if (tokens.size() != 3)
//     {
//         std::cout << "Invalid input, please refer to the example e.g. ETH/BTC,1,0.5" << std::endl;
//     }
//     else
//     {
//         try{
//             OrderBookEntry OBE = CSVReader::stringToOBE(currentTime, tokens[0], OrderBookType::ask, tokens[1], tokens[2]);
//             OBE.username = "simuser";
//             if (wallet.fulfillOrder(OBE))
//             {
//                 std::cout << "Wallet can fullful order." << std::endl;
//             }
//             else
//             {
//                 std::cout << "==Insuficient funds==" << std::endl;
//             }
//             orderBook.insertOrder(OBE);
//         }catch(const std::exception& e)
//         {
//             std::cout << "MerkelMain::makeAsk Invalid input."<< std::endl;
//         }
//     }
// }

void AutoTrader::autoBid(std::string product, std::string timestamp)
{
    /** Makes automatic bids in orderbook
     * 1. Gets all bids from order book
     * 2. Finds the cheapest bids for each product at current time point
     * 3. Creates orderbookentry and inserts order into orderbook
     *      - If ROI not reached
     *      - If stop loss not reached */ 
    std::vector<OrderBookEntry> askEntries = orderBook.getOrdersOfCurrency(OrderBookType::bid,
                                                                product, timestamp);

    double amountToTrade = currMap[product] / numberTrades;
    double lowestPrice = orderBook.getLowPrice(askEntries);
    std::cout << lowestPrice;

    for (OrderBookEntry& asks: askEntries)
    {
        if (asks.price == lowestPrice) 
        {
            // TODO: - Will need to remove order from order book once trade processed
            OrderBookEntry OBE = CSVReader::stringToOBE(timestamp, asks.product, OrderBookType::ask, std::to_string(lowestPrice), std::to_string(amountToTrade));
            orderBook.insertOrder(OBE);
        }
    }
}

void AutoTrader::autoStart()
{
    /** Loop through currMap and create a orderbook entry ask/bid
     * Keep in mind the stop loss and the ROI
     * Once stop loss reached, exit
     * Once ROI reaches, exit
     * Return the wallet amount at the end along with number of trades made
     * (May need a separte cpp for stratergies etc) */ 

    AutoTrader::autoAsk("BTC/ETH","2020/03/17 17:01:24.88449"); // FIXME: This is causing segementaiton error
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