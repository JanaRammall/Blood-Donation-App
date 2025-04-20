#ifndef BLOODUNIT_H
#define BLOODUNIT_H

#include <string>

class BloodUnit {
public:
    static bool addBloodUnit(const std::string& bloodType,
                             int quantity,
                             const std::string& expirationDate,
                             int sourceRequestID);

    static void getUnitsByType(const std::string& bloodType);
    static void removeExpiredUnits();
    static void listAllAvailable();
    static bool useBloodUnits(const std::string& bloodType, int quantity);
};

#endif // BLOODUNIT_H