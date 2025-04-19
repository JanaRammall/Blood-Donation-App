#ifndef DONOR_H
#define DONOR_H

#include <string>

class Donor {
public:
    static bool addDonor(const std::string& username,
                         const std::string& name,
                         int age,
                         const std::string& gender,
                         const std::string& bloodType,
                         const std::string& contact);

    // Displays *all* donors, newest first
    static void viewAllDonors();

    // Displays only those matching the given blood type
    static void findDonorsByBloodType(const std::string& bloodType);

    static int getDonorIDByUsername(const std::string& username);
};

#endif