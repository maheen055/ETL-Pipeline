#include "CountryData.h"
#include <iostream>
#include <string>

int main() {
    CountryData countryData;
    std::string command;
    
    while (std::cin >> command) {
        if (command == "LOAD") {
            std::string filename;
            std::cin >> filename;
            if (countryData.load(filename)) {
                std::cout << "success" << std::endl;
            }
        }
        else if (command == "BUILD") {
            std::string seriesCode;
            std::cin >> seriesCode;
            if (countryData.buildCommand(seriesCode)) {
                std::cout << "success" << std::endl;
            }
        }
        else if (command == "LIST") {
            std::string country;
            std::getline(std::cin >> std::ws, country);
            std::cout << countryData.listCommand(country) << std::endl;
        }
        else if (command == "RANGE") {
            std::string seriesCode;
            std::cin >> seriesCode;
            std::cout << countryData.rangeCommand(seriesCode) << std::endl;
        }
        else if (command == "FIND") {
            double mean;
            std::string op;
            std::cin >> mean >> op;
            std::cout << countryData.findCommand(mean, op) << std::endl;
        }
        else if (command == "DELETE") {
            std::string country;
            std::getline(std::cin >> std::ws, country);
            if (countryData.deleteCommand(country)) {
                std::cout << "success" << std::endl;
            } else {
                std::cout << "failure" << std::endl;
            }
        }
        else if (command == "LIMITS") {
            std::string condition;
            std::cin >> condition;
            std::cout << countryData.limitsCommand(condition) << std::endl;
        }
        else if (command == "LOOKUP") {
            std::string code;
            std::cin >> code;
            std::pair<int,int> sr = countryData.lookupCommand(code);
            if (sr.first == -1)
                std::cout << "failure" << std::endl;
            else
                std::cout << "index " << sr.first << " searches " << sr.second << std::endl;
        }
        else if (command == "REMOVE") {
            std::string code;
            std::cin >> code;
            if (countryData.removeCommand(code))
                std::cout << "success" << std::endl;
            else
                std::cout << "failure" << std::endl;
        }
        else if (command == "INSERT") {
            std::string code, filename;
            std::cin >> code >> filename;
            if (countryData.insertCommand(code, filename))
                std::cout << "success" << std::endl;
            else
                std::cout << "failure" << std::endl;
        }
        else if (command == "EXIT") {
            break;
        }
    }
    return 0;
}
