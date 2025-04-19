#ifndef VIEWER_H
#define VIEWER_H

#include "UserAccount.h"

class Viewer : public UserAccount {
public:
    Viewer(const std::string& uname)
      : UserAccount(uname, "viewer") {}

    void displayMenu() override;
};

#endif