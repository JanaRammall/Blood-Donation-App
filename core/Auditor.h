#ifndef AUDITOR_H
#define AUDITOR_H

#include "UserAccount.h"

class Auditor : public UserAccount {
public:
    Auditor(const std::string& uname)
      : UserAccount(uname, "audit") {}

    void displayMenu() override;
};

#endif 