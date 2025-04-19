#ifndef ADMIN_H
#define ADMIN_H

#include "UserAccount.h"

class Admin : public UserAccount {
public:
    Admin(const std::string& uname)
      : UserAccount(uname, "admin") {}

    void displayMenu() override;
};

#endif 
