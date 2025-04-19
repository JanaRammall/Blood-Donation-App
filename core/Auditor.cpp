#include "Auditor.h"
#include "../utils/MenuConfig.h"
#include "../utils/MenuUtils.h"
#include "../utils/InputUtils.h"
#include "../utils/ActionUtils.h"
using namespace Actions;
#include <iostream>

void Auditor::displayMenu() {
    bool running = true;
    while (running) {
        printMenu(AUDITOR_MENU, "Auditor Panel");
        int choice = inputInt("");
        switch (choice) {
            case 1:  viewActivityLog(); break;
            case 0:  running = false;   break;
            default: std::cout << "Invalid option.\n"; break;
        }
    }
}