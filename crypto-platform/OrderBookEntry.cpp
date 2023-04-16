#include "crypto-platform/OrderBookEntry.h"

OrderBookEntry::OrderBookEntry( 
                std::string _timestamp,
                std::string _product,
                OrderBookType _orderType,
                double _price,
                double _amount,
                std::string _username)
                
: timestamp(_timestamp),
  product(_product),
  type(_orderType),
  price(_price),
  amount(_amount),
  username(_username)
{
    
}

OrderBookType OrderBookEntry::StringToOrderBookType(std::string s)
{
  if (s == "bid")
  {
    return OrderBookType::bid;
  }
  if (s == "ask")
  {
    return OrderBookType::ask;
  }
  return OrderBookType::unknown;
}


