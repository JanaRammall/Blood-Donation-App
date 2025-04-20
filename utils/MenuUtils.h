#ifndef MENUUTILS_H
#define MENUUTILS_H

#include <iostream>
#include <string>
#include <vector>
#include "MenuConfig.h"

inline void printMenu(const std::vector<MenuSection>& sections, const std::string& title) {
    std::cout << "\n====== " << title << " ======\n";
    for (const auto& section : sections) {
        std::cout << "\n-- " << section.title << " --\n";
        for (const auto& item : section.items) {
            std::cout << item.number << ". " << item.label << "\n";
        }
    }
    std::cout << "======================================\n";
    std::cout << "Choice: ";
}

#endif