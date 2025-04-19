#ifndef DONORINFO_H
#define DONORINFO_H

#include <string>
#include <iostream>

struct DonorInfo {
    int donorID;
    std::string username, name, bloodType, contact, gender, createdAt;
    int age;

    DonorInfo(int id, const std::string& uname, const std::string& n,
        int a, const std::string& g, const std::string& bt,
        const std::string& c, const std::string& joined)
        : donorID(id), username(uname), name(n), age(a), gender(g),
        bloodType(bt), contact(c), createdAt(joined) {}

    friend std::ostream& operator<<(std::ostream& os, const DonorInfo& d) {
        os << "ID: " << d.donorID << " | Username: " << d.username
           << " | Name: " << d.name << " | Age: " << d.age
           << " | Gender: " << d.gender << " | Type: " << d.bloodType
           << " | Contact: " << d.contact << " | Joined: " << d.createdAt;
        return os;
    }
};

#endif
