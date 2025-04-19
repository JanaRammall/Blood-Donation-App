#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
    static void log(const std::string& username, const std::string& activity);
    static void viewLog();
};

#endif // LOGGER_H
