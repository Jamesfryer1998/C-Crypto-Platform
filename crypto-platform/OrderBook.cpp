#include <crypto-platform/OrderBook.h>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <map>
#include <set>

OrderBook::OrderBook(std::string fileName)
{
    orders = CSVReader::readCSV(fileName);
}

OrderBook::KnownProducts OrderBook::getKnownProducts()
{
    KnownProducts products;
    for(auto& order: orders)
    {
        products.insert(order.product);
    };
    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& order: orders)
    {
        if (order.type == type &&
            order.product == product &&
            order.timestamp == timestamp)
            {
                orders_sub.push_back(order);
            }
    }
    return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max_price = orders[0].price;
    for (OrderBookEntry& order: orders)
    {
        if (order.price > max_price) max_price = order.price;
    }
    return max_price;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min_price = orders[0].price;
    for (OrderBookEntry& order: orders)
    {
        if (order.price < min_price) min_price = order.price;
    }
    return min_price;
}

double OrderBook::getAvgPrice(std::vector<OrderBookEntry>& orders)
{
    double sum_of_elements = 0;
    int number_of_elements = orders.size();
    for (auto order: orders)
    {
        sum_of_elements += order.price;
    }
    double average_price = sum_of_elements/number_of_elements;
    return average_price;
}

void  OrderBook::testStats()
{
    int highPrice = 3;
    int lowPrice = 1;
    int avgPrice = 2;
    int spread = 2;
    std::vector<OrderBookEntry> orders;
    OrderBookEntry entry1("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,1,1);
    OrderBookEntry entry2("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,2,1);
    OrderBookEntry entry3("2020/03/17 17:01:24.884492","ETH/BTC",OrderBookType::bid,3,1);
    orders.push_back(entry1);
    orders.push_back(entry2);
    orders.push_back(entry3);

    std::cout << "\nTests:" << std::endl;

    if (OrderBook::getHighPrice(orders) == highPrice)
    {
        std::cout << "High Price Test Passed." << std::endl;
    }
    else
    {
        std::cout << "High Price Test Failed." << std::endl;
    }
    if (OrderBook::getLowPrice(orders) == lowPrice)
    {
        std::cout << "Low Price Test Passed." << std::endl;
    }
    else
    {
        std::cout << "Low Price Test Failed." << std::endl;
    }
    if (OrderBook::getHighPrice(orders) - OrderBook::getLowPrice(orders) == spread)
    {
        std::cout << "Spread Price Test Passed." << std::endl;
    }
    else
    {
        std::cout << "Spread Price Test Failed." << std::endl;
    }
    if (OrderBook::getAvgPrice(orders) == avgPrice)
    {
        std::cout << "Avg Price Test Passed." << std::endl;
    }
    else
    {
        std::cout << "Avg Price Test Failed." << std::endl;
    }

}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& order: orders)
    {
        if (order.timestamp > timestamp)
        {
            next_timestamp = order.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::comapreByTimeStamp);
}

std::vector<OrderBookEntry> OrderBook::matchSystem(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);
    std::vector<OrderBookEntry> sales;

    std::sort(asks.begin(), asks.end(), OrderBookEntry::comapareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::comapareByPriceDesc);

    for (OrderBookEntry& ask: asks)
    {
        for (OrderBookEntry& bid: bids)
        {
            if (bid.price >= ask.price)
            {
                OrderBookEntry sale{timestamp,
                                    product,
                                    OrderBookType::asksale,
                                    ask.price,
                                    0};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.type =  OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.type = OrderBookType::asksale;
                }
                if (bid.amount == ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break;
                }
                if (bid.amount > ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = bid.amount - ask.amount;
                    break;
                }
                if (bid.amount < ask.amount && bid.amount > 0)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount = ask.amount - bid.amount;
                    bid.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;
}
