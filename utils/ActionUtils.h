#ifndef ACTIONUTILS_H
#define ACTIONUTILS_H

#include "../core/Hospital.h"
#include "../core/Recipient.h"
#include "../core/DonationRequest.h"
#include "../core/BloodUnit.h"
#include "../core/Donor.h"
#include "../core/User.h"
#include "InputUtils.h"
#include "Logger.h"

namespace Actions
{

    // ─── Hospital ────────────────────────────────────────────────────────────────
    inline void addHospital(const std::string &currentUser)
    {
        auto name = inputLine("Hospital name: ");
        auto location = inputLine("Location: ");
        auto contact = inputLine("Contact: ");
        if (Hospital::addHospital(name, location, contact))
            Logger::log(currentUser, "Added hospital: " + name);
    }

    inline void viewHospitals()
    {
        Hospital::viewAllHospitals();
    }

    inline void updateHospital() {
        int id = inputInt("Hospital ID to update: ");
        std::string name = inputLine("New name: ");
        std::string location = inputLine("New location: ");
        std::string contact = inputLine("New contact: ");
        Hospital::updateHospital(id, name, location, contact);
    }
    
    inline void deleteHospital() {
        int id = inputInt("Hospital ID to delete: ");
        Hospital::deleteHospital(id);
    }

    // ─── Recipient ───────────────────────────────────────────────────────────────
    inline void addRecipient(const std::string &currentUser)
    {
        auto name = inputLine("Recipient name: ");
        auto bloodType = inputLine("Blood type: ");
        auto urgency = inputLine("Urgency level: ");
        auto contact = inputLine("Contact: ");
        auto hospital = inputLine("Hospital name: ");
        int hid = Hospital::getHospitalIDByName(hospital);
        if (hid != -1 && Recipient::addRecipient(name, bloodType, urgency, contact, hid))
            Logger::log(currentUser, "Added recipient: " + name);
    }

    inline void viewRecipients()
    {
        Recipient::viewAllRecipients();
    }

    inline void viewFulfilledRecipients()
    {
        Recipient::viewFulfilledRecipients();
    }

    inline void viewUnfulfilledRecipients()
    {
        Recipient::viewUnfulfilledRecipients();
    }

    inline void transfuseToRecipient(const std::string& currentUser) {
        int id = inputInt("Recipient ID to fulfill: ");
        int qty = inputInt("Quantity of blood needed: ");
        Recipient::fulfillRecipientNeed(id, qty);
        Logger::log(currentUser, "Transfused " + std::to_string(qty) + " units to recipient ID #" + std::to_string(id));
    }

    inline void viewUrgentUnfulfilledRecipients() {
        Recipient::viewUrgentRecipients();
    }
    
    inline void updateRecipient() {
        int id = inputInt("Recipient ID to update: ");
        std::string name = inputLine("New name: ");
        std::string bt = inputLine("New blood type: ");
        std::string urgency = inputLine("New urgency level: ");
        std::string contact = inputLine("New contact: ");
        std::string hospital = inputLine("New hospital name: ");
        int hospitalID = Hospital::getHospitalIDByName(hospital);
        Recipient::updateRecipient(id, name, bt, urgency, contact, hospitalID);
    }
    
    inline void deleteRecipient() {
        int id = inputInt("Recipient ID to delete: ");
        Recipient::deleteRecipient(id);
    }

    // ─── Donation Request ────────────────────────────────────────────────────────
    inline void createDonationRequest(const std::string &currentUser)
    {
        auto name = inputLine("Donor name: ");
        auto bloodType = inputLine("Blood type: ");
        int age = inputInt("Age: ");
        auto gender = inputLine("Gender: ");
        auto contact = inputLine("Contact: ");
        auto hospital = inputLine("Hospital name: ");
        auto date = inputLine("Scheduled date (YYYY-MM-DD): ");

        int hid = Hospital::getHospitalIDByName(hospital);
        if (hid != -1 && DonationRequest::createRequest(name, bloodType, age, gender, contact, hid, date))
            Logger::log(currentUser, "Added donation request for donor: " + name);
    }

    inline void viewAllRequests()
    {
        DonationRequest::viewAllRequests();
    }

    inline void viewPendingRequests()
    {
        DonationRequest::viewPendingRequests();
    }

    inline void viewFulfilledRequests()
    {
        DonationRequest::viewFulfilledRequests();
    }

    inline void fulfillDonationRequest(const std::string &currentUser)
    {
        int id = inputInt("Request ID to fulfill: ");
        auto bloodType = inputLine("Blood type: ");
        int qty = inputInt("Quantity: ");
        if (DonationRequest::fulfillRequest(id, bloodType, qty))
            Logger::log(currentUser, "Fulfilled request ID #" + std::to_string(id));
    }

    // ─── Donor ───────────────────────────────────────────────────────────────────
    inline void viewDonors()
    {
        Donor::viewAllDonors();
    }

    inline void viewDonorsByBloodType()
    {
        auto type = inputLine("Blood type to filter by: ");
        Donor::findDonorsByBloodType(type);
    }

    inline void updateDonor() {
        int id = inputInt("Donor ID to edit: ");
        std::string name = inputLine("New name: ");
        int age = inputInt("New age: ");
        std::string gender = inputLine("New gender: ");
        std::string bloodType = inputLine("New blood type: ");
        std::string contact = inputLine("New contact: ");
        Donor::updateDonor(id, name, age, gender, bloodType, contact);
    }
    
    inline void deleteDonor() {
        int id = inputInt("Donor ID to delete: ");
        Donor::deleteDonor(id);
    }
    

    // ─── BloodUnit ───────────────────────────────────────────────────────────────
    inline void viewBloodInventory()
    {
        BloodUnit::listAllAvailable();
    }

    inline void viewBloodByType()
    {
        auto type = inputLine("Blood type to view: ");
        BloodUnit::getUnitsByType(type);
    }

    inline void removeExpiredBlood()
    {
        BloodUnit::removeExpiredUnits();
    }

    // ─── Staff/Create Account ────────────────────────────────────────────────────
    inline void createStaffAccount(const std::string &currentUser)
    {
        auto uname = inputLine("New staff username: ");
        auto pwd = getMaskedPassword("Password: ");
        std::string role;
        while (true)
        {
            role = inputLine("Choose role (admin/staff/viewer/audit): ");
            if (role == "admin" || role == "staff" || role == "viewer" || role == "audit")
                break;
            std::cout << "⚠️ Invalid role. Try again.\n";
        }
        if (User::registerUser(uname, pwd, role))
            Logger::log(currentUser, "Created user: " + uname + " as " + role);
    }

    // ─── Logger ──────────────────────────────────────────────────────────────────
    inline void viewActivityLog()
    {
        Logger::viewLog();
    }

}

#endif