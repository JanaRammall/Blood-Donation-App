#ifndef DONATIONREQUESTINFO_H
#define DONATIONREQUESTINFO_H

#include <string>
#include <iostream>

struct DonationRequestInfo
{
    int requestID;
    std::string fullName;
    std::string bloodType;
    int age;
    std::string gender;
    std::string contact;
    std::string hospitalName;
    std::string scheduledDate;
    bool fulfilled;

    DonationRequestInfo(int id,
                        const std::string &fn,
                        const std::string &bt,
                        int a,
                        const std::string &g,
                        const std::string &c,
                        const std::string &hosp,
                        const std::string &date,
                        bool f)
        : requestID(id),
          fullName(fn),
          bloodType(bt),
          age(a),
          gender(g),
          contact(c),
          hospitalName(hosp),
          scheduledDate(date),
          fulfilled(f)
    {
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const DonationRequestInfo &r)
    {
        os << "ID: " << r.requestID
           << " | Name: " << r.fullName
           << " | Type: " << r.bloodType
           << " | Age: " << r.age
           << " | Gender: " << r.gender
           << " | Contact: " << r.contact
           << " | Hospital: " << r.hospitalName
           << " | Date: " << r.scheduledDate
           << " | Status: "
           << (r.fulfilled ? "✅ Fulfilled" : "⏳ Pending");
        return os;
    }
};

#endif