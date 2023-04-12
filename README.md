# C++ Crypto Trading Platform

How to Use
Installation
To use this platform, you will need to have C++ installed on your system. You can install it using your operating system's package manager.
You may use VSCode but you will have to install the Microsoft C++ Extension.

After installing C++, you can download the source code for this platform from GitHub:

```
git clone https://github.com/Jamesfryer1998/C-Crypto-Platform.git 
```

## Building
To build the platform, navigate to the root directory of the source code and run the following command:

```
g++ --std=c++11 main.cpp MerkelMain.cpp CSVReader.cpp OrderBook.cpp OrderBookEntry.cpp Wallet.cpp
```

This will create an executable file named ./a.out

## Usage
To use the platform, run the cryptotradingplatform executable:

```
./a.out
```

This will start the platform and display the main menu.

From the main menu, you can perform the following actions:

1: Print Help
2: Print Exchange Stats
3: Make A Ask
4: Make A Bid
5: Wallet State
6: Continue

1: Print Help
- This prints the objective of the platform.

2: Print Exchange Stats
- This prints some simple stats such as: max, min, avg and spread, for each unqiue cryptocurrency.

3: Make A Ask
- Allows you to make a ask in the form of e.g. ETH/BTC,1,1

4: Make A Bid
- Allows you to make a bid in the form of e.g. ETH/BTC,1,1

5: Wallet State
- Prints the currencies held in your wallet, if no currencies held will show your wallet is empty.

6: Continue
- This will process the current asks and bids you have placed and move the simulation to the next timeframe.

## Future ideas
This idea was conducted as part of the Coursera specialisation "User C++ to build a crypto trading platform" 
and upon completion of the 5th and final course this was the end product. However, there are a few aspects
Id like to add to this project:
- 