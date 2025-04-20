#include "Staff.h"
#include "../utils/MenuConfig.h"
#include "../utils/MenuUtils.h"
#include "../utils/InputUtils.h"
#include "../utils/ActionUtils.h"
using namespace Actions;
#include <iostream>

void Staff::displayMenu() {
    bool running = true;
    while (running) {
        printMenu(STAFF_MENU, "Staff Panel");
        int choice = inputInt("");
        switch (choice) {
            // 🏥 Hospitals
            case 1:  addHospital(username);        break;
            case 2:  viewHospitals();              break;
            case 3:  updateHospital();             break;

            // 🧑‍⚕️ Recipients
            case 4:  addRecipient(username);       break;
            case 5:  viewRecipients();             break;
            case 6:  viewUnfulfilledRecipients();  break;
            case 7:  transfuseToRecipient(username); break;
            case 8:  viewUrgentUnfulfilledRecipients(); break;
            case 9:  updateRecipient();            break;

            // 🩸 Donation Requests
            case 10: createDonationRequest(username); break;
            case 11: viewPendingRequests();        break;
            case 12: viewFulfilledRequests();      break;
            case 13: fulfillDonationRequest(username); break;

            // 📋 Donors
            case 14: viewDonors();                 break;
            case 15: viewDonorsByBloodType();      break;
            case 16: updateDonor();                break;
            case 17: deleteDonor();                break;


            // 🧪 Blood Storage
            case 18: viewBloodInventory();         break;
            case 19: removeExpiredBlood();         break;

            // 🚪 Session
            case 0: running = false;               break;

            default: std::cout << "Invalid option.\n"; break;
        }
    }
}