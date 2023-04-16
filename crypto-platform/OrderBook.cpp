#include <crypto-platform/OrderBook.h>
#include <crypto-platform/OrderBookEntry.h>
#include <crypto-platform/CSVReader.h>
#include <numeric>
#include <map>
#include <set>

OrderBook::OrderBook(const std::string& fileName)
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

OrderBook::OrderBookEntryList OrderBook::getOrders(OrderBookType type,
                                        std::string product, 
                                        std::string timestamp)
{
    OrderBookEntryList orders_sub;
    for (auto& order: orders)
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

double OrderBook::getHighPrice(const OrderBook::OrderBookEntryList& orders)
{
    return std::max_element(std::begin(orders), std::end(orders), 
                            [](const auto& a, const auto& b) { return a.price < b.price; })->price;
}

double OrderBook::getLowPrice(const OrderBook::OrderBookEntryList& orders)
{
    return std::min_element(std::begin(orders), std::end(orders), 
                            [](const auto& a, const auto& b) { return a.price < b.price; })->price;
}

double OrderBook::getAvgPrice(const OrderBook::OrderBookEntryList& orders)
{
    return std::accumulate(std::begin(orders), std::end(orders), 0.0,
                           [&orders](const auto& currentValue, const auto& order) { return currentValue + order.price / orders.size(); });
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

OrderBook::OrderBookEntryList OrderBook::matchSystem(std::string product, std::string timestamp)
{
    OrderBook::OrderBookEntryList asks = getOrders(OrderBookType::ask, product, timestamp);
    OrderBook::OrderBookEntryList bids = getOrders(OrderBookType::bid, product, timestamp);
    OrderBook::OrderBookEntryList sales;

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
