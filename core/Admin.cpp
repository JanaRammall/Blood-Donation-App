#include "Admin.h"
#include "../utils/MenuConfig.h"
#include "../utils/MenuUtils.h"
#include "../utils/InputUtils.h"
#include "../utils/ActionUtils.h"
using namespace Actions;
#include <iostream>

void Admin::displayMenu() {
    bool running = true;
    while (running) {
        printMenu(ADMIN_MENU, "Admin Panel");
        int choice = inputInt("");
        switch (choice) {
            case 1:  addHospital(username);               break;
            case 2:  viewHospitals();                     break;
            case 3:  addRecipient(username);              break;
            case 4:  viewRecipients();                    break;
            case 5:  viewUnfulfilledRecipients();         break;
            case 6:  transfuseToRecipient(username);      break;
            case 7:  viewUrgentUnfulfilledRecipients();   break;
            case 8:  createDonationRequest(username);     break;
            case 9:  viewPendingRequests();               break;
            case 10: viewFulfilledRequests();             break;
            case 11: fulfillDonationRequest(username);    break;
            case 12: viewDonors();                        break;
            case 13: viewBloodInventory();                break;
            case 14: removeExpiredBlood();                break;
            case 15: viewDonorsByBloodType();             break;
            case 16: viewActivityLog();                   break;
            case 17: createStaffAccount(username);        break;
            case 0:  running = false;                     break;
            default: std::cout << "Invalid option.\n";    break;
        }
    }
}
