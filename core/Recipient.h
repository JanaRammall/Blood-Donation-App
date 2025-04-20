#ifndef RECIPIENT_H
#define RECIPIENT_H

#include <string>

class Recipient {
public:
    static bool addRecipient(const std::string& fullName,
                             const std::string& bloodType,
                             const std::string& urgencyLevel,
                             const std::string& contact,
                             int hospitalID);

    // Lists *all* recipients
    static void viewAllRecipients();

    // Lists only unfulfilled (waiting) recipients
    static void viewUnfulfilledRecipients();

    // Lists only fulfilled recipients
    static void viewFulfilledRecipients();

    static bool markAsFulfilled(int recipientID);
    static int  getRecipientIDByName(const std::string& fullName);
    static void fulfillRecipientNeed(int recipientID, int quantity);
    static void viewUrgentRecipients();
    static bool updateRecipient(int id, const std::string& name, const std::string& bloodType,
        const std::string& urgency, const std::string& contact, int hospitalID);
    static bool deleteRecipient(int id);


};

#endif 