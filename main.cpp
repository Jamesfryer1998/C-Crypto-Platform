#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include "CSVReader.h"
#include "Wallet.h"

#pragma GCC diagnostic ignored "-Wc++11-extensions"
#pragma GCC diagnostic ignored "-Wlocal-type-template-args"

int main()
{
    MerkelMain app{};
    app.init();
}

// ==Advancements==
// From here, you can further expand the crypto-wallet application that you have developed during the specialisation. 
// For instance, you could create multiple wallets which hold different assets.
// Alternatively, you could spend more time in making the application more fit for use with additional 
// unit tests to control data, usage procedures, and operating procedures.

// To compile:
// g++ --std=c++11 main.cpp MerkelMain.cpp CSVReader.cpp OrderBook.cpp OrderBookEntry.cpp Wallet.cpp