#include <crypto-platform/AutoTrader.h>
#include <crypto-platform/OrderBook.h>
#include <crypto-platform/CSVReader.h>
#include <crypto-platform/Strategies.h>

#include <iostream>
#include <vector>
#include <map>

AutoTrader::AutoTrader()
{

}

void AutoTrader::callAutoTrader(Wallet walletInput)
{
    system("clear");
    std::cout << "===== USING NEW MATCHING SYSTEM =====\n" << std::endl;
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
        AutoTrader::generateTrades();
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
    std::cout << "Please enter your prefered % ROI: ";
    roi = getUserOption(false);

    for(const auto& key_value: currMap) {
        std::string product = key_value.first;
        double finalAmount = key_value.second * (100 + stopLossOption)/100;
        roiMap.insert({product, finalAmount});
        std::cout << product << " : " << finalAmount << std::endl;
    }
}

void AutoTrader::generateTrades()
{
    auto orderBook = match.getOrderBook();

    for (auto& curr: currMap)
    {
        auto productCurrency = match.getProductsOfCurrency(curr.first);
        double tradeAmount = strat.fixedTradeSize(100, curr.second);
        for (std::string& product: productCurrency)
        {
            auto currencyOrders = orderBook[product];

            // Find the average price for a product from asks and bids
            double histAveragePrice = (strat.calcAveragePrice(currencyOrders["orderType"]["ask"]) + 
                                       strat.calcAveragePrice(currencyOrders["orderType"]["bid"]))/2;

            double lowPrice = OrderBook::getLowPrice(currencyOrders["orderType"]["ask"]);
            double highPrice = OrderBook::getHighPrice(currencyOrders["orderType"]["bid"]);

            // Placeholder neworder
            // OrderBookEntry newOrder = {"", "", OrderBookType::ask, 1, 1};
            
            // FIXME:
            // Major design flaw, we are now looping through all the asks and then bids
            // We need a mixed orderbook, preferable as it comes in
            for (OrderBookEntry& currentOrder: currencyOrders["orderType"]["ask"], currencyOrders["orderType"]["bid"])
            {
                int meanRev = strat.meanReversion(currentOrder, histAveragePrice);

                if (meanRev == 0)
                {
                    OrderBookEntry newOrder(currentOrder.timestamp,
                                            currentOrder.product,
                                            OrderBookType::ask,
                                            lowPrice,
                                            tradeAmount);

                    match.matchEngine(newOrder);

                    // std::cout << "Sell trade" << std::endl;
                }
                else if (meanRev == 1)
                {
                    OrderBookEntry newOrder(currentOrder.timestamp,
                                            currentOrder.product,
                                            OrderBookType::bid,
                                            highPrice,
                                            tradeAmount);
                    match.matchEngine(newOrder);

                    // std::cout << "Buy trade" << std::endl;
                }
                else if (meanRev == 2) continue;

            }
            auto trades = match.getTrades();
            std::cout << "num trades: " << trades.size() << std::endl;

            // TODO:
            // - Remove currency from currency wallet per trade with amount from trades
        }
    }
}


/** Makes automatic asks in orderbook
 * 1. Gets all asks from order book
 * 2. Finds the cheapest asks for each product at current time point
 * 3. Creates orderbookentry and inserts order into orderbook
 *      - If ROI not reached
 *      - If stop loss not reached */ 
void AutoTrader::autoStart()
{
    AutoTrader::generateTrades();
    /** Loop through currMap and create a orderbook entry ask/bid
     * Keep in mind the stop loss and the ROI
     * Once stop loss reached, exit
     * Once ROI reaches, exit
     * Return the wallet amount at the end along with number of trades made
     * (May need a separte cpp for stratergies etc) */ 
}

void AutoTrader::autoStop()
{

}

bool AutoTrader::determineEnd()
{
    /** When suitable end found
     * - either stop loss reached
     * - or, ROI achieved*/ 
    return false;
}