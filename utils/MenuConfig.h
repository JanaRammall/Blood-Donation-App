#ifndef MENUCONFIG_H
#define MENUCONFIG_H

#include <vector>
#include <string>
#include <utility>

struct MenuItem {
    int number;
    std::string label;
};

struct MenuSection {
    std::string title;
    std::vector<MenuItem> items;
};

inline const std::vector<MenuSection> VIEWER_MENU = {
    {"🔍 View Data", {
        {1, "View Blood Inventory"},
        {2, "View All Recipients"},
        {3, "View Pending Requests"},
        {4, "View Donors"},
        {0, "Logout"}
    }}
};

inline const std::vector<MenuSection> AUDITOR_MENU = {
    {"📜 Audit Tools", {
        {1, "View Activity Log"},
        {0, "Logout"}
    }}
};

inline const std::vector<MenuSection> STAFF_MENU = {
    {"🏥 Hospitals", {
        {1, "Add Hospital"},
        {2, "View Hospitals"},
        {3, "Edit Hospital"}
    }},
    {"🧑‍⚕️ Recipients", {
        {4, "Add Recipient"},
        {5, "View All Recipients"},
        {6, "View Unfulfilled Recipients"},
        {7, "Transfuse to Recipient"},
        {8, "View Urgent Unfulfilled Recipients"},
        {9, "Edit Recipient"}
    }},
    {"🩸 Donation Requests", {
        {10, "Add Donation Request"},
        {11, "View Pending Requests"},
        {12, "View Fulfilled Requests"},
        {13, "Fulfill Donation Request"}
    }},
    {"📋 Donors", {
        {14, "View Donors"},
        {15, "Find Donors by Blood Type"},
        {16, "Edit Donor"},
        {17, "Delete Donor"}

    }},
    {"🧪 Blood Storage", {
        {18, "View Blood Inventory"},
        {19, "Remove expired blood units"}
    }},
    {"🚪 Session", {
        {0, "Logout"}
    }}
};    

inline const std::vector<MenuSection> ADMIN_MENU = {
    {"🏥 Hospital Management", {
        {1, "Add Hospital"},
        {2, "View Hospitals"},
        {3, "Edit Hospital"},
        {4, "Delete Hospital"},
    }},
    {"🩸 Recipient Management", {
        {5, "Add Recipient"},
        {6, "View All Recipients"},
        {7, "View Unfulfilled Recipients"},
        {8, "View Urgent Recipients"},
        {9, "Edit Recipient"},
        {10, "Delete Recipient"},
        {11, "Transfuse to Recipient"},
    }},
    {"🧑‍🤝‍🧑 Donor Management", {
        {12, "View Donors"},
        {13, "Find Donors by Blood Type"},
        {14, "Edit Donor"},
        {15, "Delete Donor"},

    }},
    {"🗓️ Donation Requests", {
        {16, "Add Donation Request"},
        {17, "View Pending Requests"},
        {18, "View Fulfilled Requests"},
        {19, "Fulfill Donation Request"},
    }},
    {"📦 Blood Inventory", {
        {20, "View Blood Inventory"},
        {21, "Remove Expired Units"},
    }},
    {"👤 System", {
        {22, "View Activity Log"},
        {23, "Create Staff Account"},
        {0, "Logout"},
    }}
};
    
#endif