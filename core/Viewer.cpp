#include "Viewer.h"
#include "../utils/MenuConfig.h"
#include "../utils/MenuUtils.h"
#include "../utils/InputUtils.h"
#include "../utils/ActionUtils.h"
using namespace Actions;
#include <iostream>

void Viewer::displayMenu() {
    bool running = true;
    while (running) {
        printMenu(VIEWER_MENU, "Viewer Panel");
        int choice = inputInt("");
        switch (choice) {
            case 1:  viewBloodInventory(); break;
            case 2:  viewRecipients();      break;
            case 3:  viewPendingRequests(); break;
            case 4:  viewDonors();          break;
            case 0:  running = false;       break;
            default: std::cout << "Invalid option.\n"; break;
        }
    }
}