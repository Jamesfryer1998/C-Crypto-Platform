#pragma once 

#include <crypto-platform/OrderBookEntry.h>
#include <vector>

class CSVReader{

    public:
        CSVReader();

        // CSV Input Operations
        static std::vector<OrderBookEntry> readCSV(std::string fileName);
        static std::vector<std::string> tokenise(std::string csvline, char seperator);
        static OrderBookEntry stringToOBE(std::string timestamp,
                                            std::string product, 
                                            OrderBookType type,
                                            std::string price,
                                            std::string amount);

    private:
        OrderBookEntry csvStringToOBE(std::vector<std::string> inputString);


};