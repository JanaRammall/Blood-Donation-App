#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>

class Hospital {
public:
    static bool addHospital(const std::string& name, const std::string& location, const std::string& contact);
    static void viewAllHospitals();
    static int getHospitalIDByName(const std::string& name);
    static std::string getHospitalNameByID(int hospitalID);
    static bool updateHospital(int id, const std::string& name, const std::string& location, const std::string& contact);
    static bool canDeleteHospital(int hospitalID);
    static bool deleteHospital(int id);
};

#endif // HOSPITAL_H