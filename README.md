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
To build the platform install conda and create and activate the environment defined in env.yaml. 
Navigate to the root directory of the source code and prepare the build directory:
```
cmake -B build # Creates the makefile, re-run from root when CMakeFiles.txt change.
```

Once done, build the platform:
```
 cmake --build build
```

Run the tests:
```
ctest --test-dir build
```

This will create an executable file `./build/apps/crypto-platform-app`

## Usage
To use the platform, run the executable:

```
`./build/apps/crypto-platform-app`
```

This will start the platform and display the main menu.

From the main menu, you can perform the following actions:

1: Print Help<br>
2: Print Exchange Stats<br>
3: Make A Ask<br>
4: Make A Bid<br>
5: Wallet State<br>
6: Continue
7: AutoTrader Options

### 1: Print Help
- This prints the objective of the platform.

### 2: Print Exchange Stats
- This prints some simple stats such as: max, min, avg and spread, for each unqiue cryptocurrency.

### 3: Make A Ask
- Allows you to make a ask in the form of e.g. ETH/BTC,1,1

### 4: Make A Bid
- Allows you to make a bid in the form of e.g. ETH/BTC,1,1

### 5: Wallet State
- Prints the currencies held in your wallet, if no currencies held will show your wallet is empty.

### 6: Continue
- This will process the current asks and bids you have placed and move the simulation to the next timeframe.

## Future ideas

This idea was conducted as part of the Coursera specialisation "Use C++ to build a crypto trading platform" 
and upon completion of the 5th and final course this was the end product. However, there are a few aspects
Id like to add to this project:
- Create a automated trading platform
- Incorporate more advances statistical measurements
- Increase the input file count to process more ask/bids and a wider range of cryptos
- Attempt multi-exchange arbitrage