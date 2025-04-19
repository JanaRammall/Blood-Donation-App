#include "DonationRequest.h"
#include "BloodUnit.h"
#include "Donor.h"
#include "../db/Database.h"
#include "DonationRequestInfo.h"
#include "../utils/TemplateUtils.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

constexpr int BLOOD_SHELF_LIFE_DAYS = 42;

// 📅 Calculate expiration based on scheduled donation date
std::string calculateExpirationDate(const std::string &scheduledDate)
{
    std::tm tm = {};
    std::istringstream ss(scheduledDate);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    if (ss.fail())
    {
        std::cerr << "❌ Failed to parse scheduled date. Falling back to current time.\n";
        time_t fallbackTime = time(nullptr) + BLOOD_SHELF_LIFE_DAYS * 24 * 60 * 60;

        char fallback[11];
        strftime(fallback, sizeof(fallback), "%Y-%m-%d", localtime(&fallbackTime));
        return std::string(fallback);
    }

    time_t t = mktime(&tm);
    t += BLOOD_SHELF_LIFE_DAYS * 24 * 60 * 60;

    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&t));
    return std::string(buffer);
}

// ─── Extractor for one DonationRequest row ───────────────────────────────────
static DonationRequestInfo extractRequestRow(sqlite3_stmt *s)
{
    int id = getColInt(s, 0);
    std::string fn = getColText(s, 1);
    std::string bt = getColText(s, 2);
    int age = getColInt(s, 3);
    std::string g = getColText(s, 4);
    std::string c = getColText(s, 5);
    std::string hosp = getColText(s, 6); // hospital name joined in SQL
    std::string date = getColText(s, 7);
    bool ful = getColInt(s, 8) != 0;

    return DonationRequestInfo{
        id, fn, bt, age, g, c, hosp, date, ful};
}

// ✍️ Create a new donation request
bool DonationRequest::createRequest(const std::string &fullName,
                                    const std::string &bloodType,
                                    int age,
                                    const std::string &gender,
                                    const std::string &contact,
                                    int hospitalID,
                                    const std::string &scheduledDate)
{
    sqlite3 *db = Database::getDB();
    std::string sql =
        "INSERT INTO DonationRequest (fullName, bloodType, age, gender, contact, hospitalID, scheduledDate) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);

    sqlite3_bind_text(stmt, 1, fullName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, bloodType.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, gender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, contact.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, hospitalID);
    sqlite3_bind_text(stmt, 7, scheduledDate.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        std::cout << "✅ Donation request created successfully.\n";
        sqlite3_finalize(stmt);
        return true;
    }
    else
    {
        std::cerr << "❌ Failed to create donation request: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

// ✅ Fulfill a donation request: add blood unit + register donor
bool DonationRequest::fulfillRequest(int requestID, const std::string &bloodType, int quantity)
{
    sqlite3 *db = Database::getDB();

    std::string sql =
        "SELECT fullName, age, gender, contact, fulfilled, scheduledDate "
        "FROM DonationRequest WHERE requestID = ?";

    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_int(stmt, 1, requestID);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        std::cerr << "❌ Donation request not found.\n";
        sqlite3_finalize(stmt);
        return false;
    }

    std::string fullName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    int age = sqlite3_column_int(stmt, 1);
    std::string gender = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    std::string contact = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    int fulfilled = sqlite3_column_int(stmt, 4);
    std::string scheduledDate = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    sqlite3_finalize(stmt);

    if (fulfilled == 1)
    {
        std::cerr << "⚠️  Donation request already fulfilled.\n";
        return false;
    }

    // Mark as fulfilled
    std::string updateSQL = "UPDATE DonationRequest SET fulfilled = 1 WHERE requestID = ?";
    sqlite3_stmt *updateStmt;
    Database::prepareStatement(db, updateStmt, updateSQL);
    sqlite3_bind_int(updateStmt, 1, requestID);

    if (sqlite3_step(updateStmt) != SQLITE_DONE)
    {
        std::cerr << "❌ Failed to update fulfillment status.\n";
        sqlite3_finalize(updateStmt);
        return false;
    }
    sqlite3_finalize(updateStmt);

    // Create blood unit using scheduled date
    std::string expirationDate = calculateExpirationDate(scheduledDate);
    bool added = BloodUnit::addBloodUnit(bloodType, quantity, expirationDate, requestID);

    // Add donor to system
    std::string pseudoUsername = "donor_" + std::to_string(requestID);
    Donor::addDonor(pseudoUsername, fullName, age, gender, bloodType, contact);

    return added;
}

// 👁️ View all requests (admin only)
void DonationRequest::viewAllRequests()
{
    static const std::string sql =
        "SELECT D.requestID,"
        " D.fullName,"
        " D.bloodType,"
        " D.age,"
        " D.gender,"
        " D.contact,"
        " H.name," // hospital name
        " D.scheduledDate,"
        " D.fulfilled"
        " FROM DonationRequest D"
        " JOIN Hospital H ON D.hospitalID = H.hospitalID"
        " ORDER BY D.scheduledDate ASC;";

    viewByQuery<DonationRequestInfo>(
        sql,
        "📄 All Donation Requests",
        extractRequestRow);
}

// 🔍 Helper to print pending/fulfilled only
void DonationRequest::viewRequestsByStatus(bool fulfilledStatus,
                                           const std::string &label)
{
    static const std::string sql =
        "SELECT D.requestID,"
        " D.fullName,"
        " D.bloodType,"
        " D.age,"
        " D.gender,"
        " D.contact,"
        " H.name,"
        " D.scheduledDate,"
        " D.fulfilled"
        " FROM DonationRequest D"
        " JOIN Hospital H ON D.hospitalID = H.hospitalID"
        " WHERE D.fulfilled = ?"
        " ORDER BY D.scheduledDate ASC;";

    viewByQuery<DonationRequestInfo>(
        sql,
        label,
        extractRequestRow,
        std::to_string(fulfilledStatus) // will bind to the '?' as 0 or 1
    );
}

// Public APIs to filter by status
void DonationRequest::viewPendingRequests()
{
    viewRequestsByStatus(false, "⏳ Pending Donation Requests:");
}

void DonationRequest::viewFulfilledRequests()
{
    viewRequestsByStatus(true, "✅ Fulfilled Donation Requests:");
}
