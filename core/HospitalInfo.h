#ifndef HOSPITALINFO_H
#define HOSPITALINFO_H

#include <string>
#include <iostream>

struct HospitalInfo {
    int hospitalID;
    std::string name, location, contact;

    HospitalInfo(int id, const std::string& n, const std::string& l, const std::string& c)
        : hospitalID(id), name(n), location(l), contact(c) {}

    friend std::ostream& operator<<(std::ostream& os, const HospitalInfo& h) {
        os << "ID: " << h.hospitalID << " | Name: " << h.name
           << " | Location: " << h.location << " | Contact: " << h.contact;
        return os;
    }
};

#endif
