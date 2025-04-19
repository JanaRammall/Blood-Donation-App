#ifndef MENUUTILS_H
#define MENUUTILS_H

#include <iostream>
#include <vector>
#include <utility>
#include <string>

inline void printMenu(const std::vector<std::pair<int,std::string>>& menu, 
                      const std::string& title) {
    std::cout << "\n====== " << title << " ======\n";
    for (auto& opt : menu) {
        std::cout << opt.first << ". " << opt.second << "\n";
    }
    std::cout << "======================================\n";
    std::cout << "Choice: ";
}

#endif 