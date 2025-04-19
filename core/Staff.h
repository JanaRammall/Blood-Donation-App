#ifndef STAFF_H
#define STAFF_H

#include "UserAccount.h"

class Staff : public UserAccount {
public:
    Staff(const std::string& uname)
      : UserAccount(uname, "staff") {}

    void displayMenu() override;
};

#endif
