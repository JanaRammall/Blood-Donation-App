#ifndef DONATIONREQUEST_H
#define DONATIONREQUEST_H

#include <string>

class DonationRequest {
public:
    static bool createRequest(const std::string& fullName,
                              const std::string& bloodType,
                              int age,
                              const std::string& gender,
                              const std::string& contact,
                              int hospitalID,
                              const std::string& scheduledDate);

    static void viewAllRequests();
    static void viewRequestsByStatus(bool fulfilledStatus, const std::string& label);
    static void viewPendingRequests();
    static void viewFulfilledRequests();

    static bool fulfillRequest(int requestID, const std::string& bloodType, int quantity);
};

#endif
