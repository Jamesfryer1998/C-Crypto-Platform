#pragma once
#include <iostream>
#include <string>
#include <vector>

enum class OrderBookType{bid, ask, asksale, bidsale, unknown};

class OrderBookEntry
{
    public:
        OrderBookEntry( std::string _timestamp,
                        std::string _product,
                        OrderBookType _orderType,
                        double _price,
                        double _amount,
                        std::string username = "dataset");

        static OrderBookType StringToOrderBookType(std::string s);

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
        
        std::string timestamp;
        std::string product;
        OrderBookType type;
        double price;
        double amount;
        std::string username;
};