#ifndef MENUCONFIG_H
#define MENUCONFIG_H

#include <vector>
#include <string>
#include <utility>

inline const std::vector<std::pair<int, std::string>> VIEWER_MENU = {
    {1, "View Blood Inventory"},
    {2, "View All Recipients"},
    {3, "View Pending Requests"},
    {4, "View Donors"},
    {0, "Logout"}
};

inline const std::vector<std::pair<int, std::string>> AUDITOR_MENU = {
    {1, "View Activity Log"},
    {0, "Logout"}};

inline const std::vector<std::pair<int, std::string>> STAFF_MENU = {
    {1, "Add Hospital"},
    {2, "View Hospitals"},
    {3, "Add Recipient"},
    {4, "View All Recipients"},
    {5, "View Unfulfilled Recipients"},
    {6, "Transfuse to Recipient"},
    {7, "View Urgent Unfulfilled Recipients"},
    {8, "Add Donation Request"},
    {9, "View Pending Requests"},
    {10, "View Fulfilled Requests"},
    {11, "Fulfill Donation Request"},
    {12, "View Donors"},
    {13, "View Blood Inventory"},
    {14, "Remove expired blood units"},
    {15, "Find Donors by Blood Type"},
    {0, "Logout"}};

inline const std::vector<std::pair<int, std::string>> ADMIN_MENU = {
    {1, "Add Hospital"},
    {2, "View Hospitals"},
    {3, "Add Recipient"},
    {4, "View All Recipients"},
    {5, "View Unfulfilled Recipients"},
    {6, "Transfuse to Recipient"},
    {7, "View Urgent Unfulfilled Recipients"},
    {8, "Add Donation Request"},
    {9, "View Pending Requests"},
    {10, "View Fulfilled Requests"},
    {11, "Fulfill Donation Request"},
    {12, "View Donors"},
    {13, "View Blood Inventory"},
    {14, "Remove expired blood units"},
    {15, "Find Donors by Blood Type"},
    {16, "View Activity Log"},
    {17, "Create Staff Account"},
    {0, "Logout"}};

#endif