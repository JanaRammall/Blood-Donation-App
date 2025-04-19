#ifndef BLOODUNITINFO_H
#define BLOODUNITINFO_H

#include <string>
#include <iostream>

struct BloodUnitInfo {
    int bloodID;
    std::string bloodType;
    int quantity;
    std::string expirationDate;

    BloodUnitInfo(int id, const std::string& type, int qty, const std::string& exp)
        : bloodID(id), bloodType(type), quantity(qty), expirationDate(exp) {}

    friend std::ostream& operator<<(std::ostream& os, const BloodUnitInfo& b) {
        os << "ID: " << b.bloodID << " | Type: " << b.bloodType
           << " | Qty: " << b.quantity << " | Exp: " << b.expirationDate;
        return os;
    }
};

#endif