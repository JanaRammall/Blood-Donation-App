#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <string>

class UserAccount {
protected:
    std::string username;
    std::string role;

public:
    UserAccount(const std::string& uname, const std::string& r)
        : username(uname), role(r) {}

    virtual ~UserAccount() {}

    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }

    virtual void displayMenu() = 0;
};

#endif
