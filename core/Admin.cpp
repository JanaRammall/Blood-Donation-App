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
            // 🏥 Hospital Management DONE
            case 1:  addHospital(username);           break;
            case 2:  viewHospitals();                 break;
            case 3:  updateHospital();                break;
            case 4:  deleteHospital();                break;

            // 🩸 Recipient Management
            case 5:  addRecipient(username);          break;
            case 6:  viewRecipients();                break;
            case 7:  viewUnfulfilledRecipients();     break;
            case 8:  viewUrgentUnfulfilledRecipients(); break;
            case 9:  updateRecipient();               break;
            case 10: deleteRecipient();               break;
            case 11: transfuseToRecipient(username);  break;

            // 🧑‍🤝‍🧑 Donor Management
            case 12: viewDonors();                    break;
            case 13: viewDonorsByBloodType();         break;
            case 14: updateDonor();                   break;
            case 15: deleteDonor();                   break;


            // 🗓️ Donation Requests
            case 16: createDonationRequest(username); break;
            case 17: viewPendingRequests();           break;
            case 18: viewFulfilledRequests();         break;
            case 19: fulfillDonationRequest(username);break;

            // 📦 Blood Inventory
            case 20: viewBloodInventory();            break;
            case 21: removeExpiredBlood();            break;

            // 👤 System
            case 22: viewActivityLog();               break;
            case 23: createStaffAccount(username);    break;

            // Exit
            case 0:  running = false; break;
            default: std::cout << "Invalid option.\n"; break;
        }
    }
}