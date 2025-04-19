#ifndef RECIPIENTINFO_H
#define RECIPIENTINFO_H

#include <string>
#include <iostream>

struct RecipientInfo {
    int recipientID;
    std::string name, bloodType, urgencyLevel, contact, hospitalName, status;

    RecipientInfo(int id, const std::string& n, const std::string& bt,
                  const std::string& urgency, const std::string& c,
                  const std::string& h, const std::string& s)
        : recipientID(id), name(n), bloodType(bt), urgencyLevel(urgency),
          contact(c), hospitalName(h), status(s) {}

    friend std::ostream& operator<<(std::ostream& os, const RecipientInfo& r) {
        os << "ID: " << r.recipientID << " | Name: " << r.name
           << " | Type: " << r.bloodType << " | Urgency: " << r.urgencyLevel
           << " | Contact: " << r.contact << " | Hospital: " << r.hospitalName
           << " | Status: " << r.status;
        return os;
    }
};

#endif