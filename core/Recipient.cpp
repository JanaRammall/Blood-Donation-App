#include "Recipient.h"
#include "../db/Database.h"
#include "RecipientInfo.h"
#include "../utils/TemplateUtils.h" // brings in vector + printList<>
#include "Hospital.h"
#include "BloodUnit.h"
#include <iostream>

// ─── Extractor for one Recipient row ─────────────────────────────────────────
static RecipientInfo extractRecipientRow(sqlite3_stmt *s)
{
    int id = getColInt(s, 0);
    std::string name = getColText(s, 1);
    std::string bt = getColText(s, 2);
    std::string urg = getColText(s, 3);
    std::string cnt = getColText(s, 4);
    int hid = getColInt(s, 5);
    bool ful = getColInt(s, 6);

    return RecipientInfo{
        id,
        name,
        bt,
        urg,
        cnt,
        Hospital::getHospitalNameByID(hid),
        ful ? "✅ Fulfilled" : "⏳ Waiting"};
}

bool Recipient::addRecipient(const std::string &fullName,
                             const std::string &bloodType,
                             const std::string &urgencyLevel,
                             const std::string &contact,
                             int hospitalID)
{
    sqlite3 *db = Database::getDB();
    std::string sql =
        "INSERT INTO Recipient (name, bloodType, urgencyLevel, contact, hospitalID) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, fullName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, bloodType.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, urgencyLevel.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, contact.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, hospitalID);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        std::cout << "✅ Recipient added: " << fullName << "\n";
        sqlite3_finalize(stmt);
        return true;
    }
    else
    {
        std::cerr << "❌ Failed to add recipient: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

void Recipient::viewAllRecipients()
{
    static const std::string sql =
        "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
        "FROM Recipient ORDER BY createdAt DESC;";
    viewByQuery<RecipientInfo>(
        sql,
        "📋 All Recipients",
        extractRecipientRow);
}

void Recipient::viewUnfulfilledRecipients()
{
    static const std::string sql =
        "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
        "FROM Recipient WHERE fulfilled=0 ORDER BY urgencyLevel DESC;";
    viewByQuery<RecipientInfo>(
        sql,
        "🆘 Recipients Awaiting Blood",
        extractRecipientRow);
}

void Recipient::viewFulfilledRecipients()
{
    static const std::string sql =
        "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
        "FROM Recipient WHERE fulfilled=1 ORDER BY createdAt DESC;";
    viewByQuery<RecipientInfo>(
        sql,
        "✅ Fulfilled Recipients",
        extractRecipientRow);
}

bool Recipient::markAsFulfilled(int recipientID)
{
    sqlite3 *db = Database::getDB();
    const std::string sql =
        "UPDATE Recipient SET fulfilled = 1 WHERE recipientID = ?;";
    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_int(stmt, 1, recipientID);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        std::cout << "✅ Recipient marked as fulfilled.\n";
        sqlite3_finalize(stmt);
        return true;
    }
    else
    {
        std::cerr << "❌ Failed to update recipient: "
                  << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

int Recipient::getRecipientIDByName(const std::string &fullName)
{
    sqlite3 *db = Database::getDB();
    const std::string sql =
        "SELECT recipientID FROM Recipient WHERE name = ? LIMIT 1;";
    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, fullName.c_str(), -1, SQLITE_STATIC);

    int recipientID = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        recipientID = sqlite3_column_int(stmt, 0);
    }
    else
    {
        std::cerr << "❌ Recipient \"" << fullName << "\" not found.\n";
    }

    sqlite3_finalize(stmt);
    return recipientID;
}

//!update
void Recipient::fulfillRecipientNeed(int recipientID, int quantity) {
    sqlite3* db = Database::getDB();

    std::string sql = "SELECT bloodType, fulfilled FROM Recipient WHERE recipientID = ?";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_int(stmt, 1, recipientID);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "❌ Recipient not found.\n";
        sqlite3_finalize(stmt);
        return;
    }

    std::string bloodType = getColText(stmt, 0);
    int fulfilled = sqlite3_column_int(stmt, 1);
    sqlite3_finalize(stmt);

    if (fulfilled) {
        std::cout << "⚠️ Recipient already fulfilled.\n";
        return;
    }

    if (!BloodUnit::useBloodUnits(bloodType, quantity)) {
        std::cout << "❌ Not enough compatible blood units available.\n";
        return;
    }

    std::string updateSQL = "UPDATE Recipient SET fulfilled = 1 WHERE recipientID = ?";
    Database::prepareStatement(db, stmt, updateSQL);
    sqlite3_bind_int(stmt, 1, recipientID);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        std::cout << "✅ Transfusion recorded and recipient marked fulfilled.\n";
    else
        std::cerr << "❌ Failed to update recipient.\n";

    sqlite3_finalize(stmt);
}

//!update
void Recipient::viewUrgentRecipients() {
    static const std::string sql =
        "SELECT recipientID, name, bloodType, urgencyLevel, contact, hospitalID, fulfilled "
        "FROM Recipient WHERE urgencyLevel = 'High' AND fulfilled = 0 ORDER BY createdAt DESC;";
    
    viewByQuery<RecipientInfo>(
        sql,
        "🚨 Urgent Unfulfilled Recipients",
        extractRecipientRow
    );
}
