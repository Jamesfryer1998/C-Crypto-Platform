#include <iostream>
#include <crypto-platform/TradeBookEntry.h>

TradeBookEntry::TradeBookEntry( std::string _timestamp,
                std::string _buyCurrency,
                std::string _sellCurrency,
                double _buyPrice,
                double _sellPrice)

: timestamp(_timestamp),
  buyCurrency(_buyCurrency),
  sellCurrency(_sellCurrency),
  buyPrice(_buyPrice),
  sellPrice(_sellPrice)
{

}