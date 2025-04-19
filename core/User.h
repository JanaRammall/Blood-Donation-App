#ifndef USER_H
#define USER_H

#include <string>

class User {
public:
    static bool registerUser(const std::string& username,
                             const std::string& password,
                             const std::string& role);

    static bool authenticate(const std::string& username,
                             const std::string& password,
                             std::string& outRole);
};

#endif // USER_H
