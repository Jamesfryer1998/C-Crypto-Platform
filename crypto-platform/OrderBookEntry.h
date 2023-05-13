#pragma once
#include <iostream>
#include <string>
#include <vector>


enum class OrderBookType{bid, ask, asksale, bidsale, unknown};

/** Timestamp
 * Product
 * OrderType
 * Price 
 * Amount */
class OrderBookEntry
{
    public:
        OrderBookEntry( std::string _timestamp,
                        std::string _product,
                        OrderBookType _orderType,
                        double _price,
                        double _amount,
                        std::string username = "dataset");

        // OrderBookEntry Operations
        static OrderBookType StringToOrderBookType(std::string s);

        // Sorting Algorithms
        static bool comapreByTimeStamp(OrderBookEntry& order1, OrderBookEntry& order2)
        {
            return order1.timestamp < order2.timestamp;
        }
        static bool comapareByPriceAsc(OrderBookEntry& order1, OrderBookEntry& order2)
        {
            return order1.price < order2.price;
        }
        static bool comapareByPriceDesc(OrderBookEntry& order1, OrderBookEntry& order2)
        {
            return order1.price > order2.price;
        }
        static bool comparePriceTimestamp(const OrderBookEntry& order1, const OrderBookEntry& order2) 
        {
            if (order1.price == order2.price)
            {
                if (order1.timestamp == order2.timestamp) 
                {
                    return order1.amount > order2.amount;
                } else {
                    return order1.timestamp < order2.timestamp;
                }
            } else 
            {
                return order1.price < order2.price;
            }
        }
        
        // Input Variables
        std::string timestamp;
        std::string product;
        OrderBookType type;
        double price;
        double amount;
        std::string username;
};