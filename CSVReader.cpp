#include "CSVReader.h"
#include "OrderBookEntry.h"
#include <iostream>
#include <fstream>

CSVReader::CSVReader()
{

}

std::vector<OrderBookEntry> CSVReader::readCSV(std::string fileName)
{
    std::vector<OrderBookEntry> entries;

    std::ifstream csvFile{fileName};
    std::string line;
    std::vector<std::string> tokens;

    if (csvFile.is_open())
    {
        int bad_count = 0;
        int good_count = 0;
        std::cout << "File open." << std::endl;
        while (std::getline(csvFile, line))
        {
            try{
                CSVReader csv;
                OrderBookEntry OBE = csv.csvStringToOBE(tokenise(line, ','));
                entries.push_back(OBE);
            }catch(const std::exception& e)
            {
                std::cout << "CSVReader::readCSV bad data processed"  << std::endl;
            }
            
        }
    }

    std::cout << "CSVReader::readCSV read" << entries.size() << "files" << std::endl;
    return entries;
}

std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator)
{
    std::vector<std::string> tokens;
    
    signed int start, end;
    std::string token;
    start = csvLine.find_first_not_of(separator, 0);

    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break;
        if (end >= 0 ) token = csvLine.substr(start, end - start);
        else token = csvLine.substr(start, csvLine.length() - start);
        tokens.push_back(token);
        start = end + 1;
    }while(end != std::string::npos);

    return tokens;
}

OrderBookEntry CSVReader::csvStringToOBE(std::vector<std::string> tokens)
{
    double price, amount; 

    if (tokens.size() != 5) 
    {
        std::cout << "Invalid CSV line.";
        throw std::exception{};
        // bad_count += 1;
    }
    try{
        std::string timestamp = tokens[0];
        std::string product = tokens[1];
        std::string orderType = tokens[2];
        price = std::stod(tokens[3]);
        amount = std::stod(tokens[4]);
        // good_count += 1;
    }catch(const std::exception& e)
    {
        std::cout << "Bad data." << std::endl;
        // bad_count += 1;
        throw;
    }
    OrderBookEntry OBE{tokens[0], tokens[1], OrderBookEntry::StringToOrderBookType(tokens[2]), price, amount};
    return OBE;
}

OrderBookEntry CSVReader::stringToOBE(std::string timestamp,
                             std::string product, 
                             OrderBookType type,
                             std::string priceString,
                             std::string amountString)
{
    double price, amount;
    try{
        price = std::stod(priceString);
        amount = std::stod(amountString);
    }catch(const std::exception& e)
    {
        std::cout << "CSVReader::csvStringToOBE Bad data." << std::endl;
        throw;
    }

     OrderBookEntry OBE{timestamp, product, type, price, amount};
    return OBE;
}
