#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>

class Hospital {
public:
    static bool addHospital(const std::string& name, const std::string& location, const std::string& contact);
    static void viewAllHospitals();
    static int getHospitalIDByName(const std::string& name);
    static std::string getHospitalNameByID(int hospitalID);
};

#endif // HOSPITAL_H
