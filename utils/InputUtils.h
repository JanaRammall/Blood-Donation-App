#ifndef INPUTUTILS_H
#define INPUTUTILS_H
#ifdef _WIN32
  #include <conio.h>
#endif

#include <iostream>
#include <string>
#include <limits>

inline int inputInt(const std::string& prompt) {
    int val;
    std::cout << prompt;
    while (!(std::cin >> val)) {
        std::cout << "Invalid input. Try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

inline std::string inputLine(const std::string& prompt) {
    std::string val;
    std::cout << prompt;
    std::getline(std::cin, val);
    return val;
}

inline std::string getMaskedPassword(const std::string& prompt) {
    std::string pwd;
    std::cout << prompt;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!pwd.empty()) {
                pwd.pop_back();
                std::cout << "\b \b";
            }
        } else if (isprint(ch)) {
            pwd += ch;
            std::cout << '*';
        }
    }
    std::cout << '\n';
    return pwd;
}

#endif 