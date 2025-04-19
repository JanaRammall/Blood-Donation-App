#include "Donor.h"
#include "../db/Database.h"
#include "DonorInfo.h"
#include "../utils/TemplateUtils.h"
#include <iostream>

// ─── Row extractor ────────────────────────────────────────────────────────────
static DonorInfo extractDonorRow(sqlite3_stmt *s)
{
    return DonorInfo{
        getColInt(s, 0),  // donorID
        getColText(s, 1), // username
        getColText(s, 2), // name
        getColInt(s, 3),  // age
        getColText(s, 4), // gender
        getColText(s, 5), // bloodType
        getColText(s, 6), // contact
        getColText(s, 7)  // createdAt
    };
}

bool Donor::addDonor(const std::string &username,
                     const std::string &name,
                     int age,
                     const std::string &gender,
                     const std::string &bloodType,
                     const std::string &contact)
{
    sqlite3 *db = Database::getDB();

    if (Database::recordExists("Donor", "username", username))
    {
        std::cerr << "⚠️ Donor already exists with username: " << username << "\n";
        return false;
    }

    std::string sql =
        "INSERT INTO Donor (username, name, age, gender, bloodType, contact) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, age);
    sqlite3_bind_text(stmt, 4, gender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, bloodType.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, contact.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        std::cout << "✅ Donor " << name << " added successfully.\n";
        sqlite3_finalize(stmt);
        return true;
    }
    else
    {
        std::cerr << "❌ Failed to add donor: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

void Donor::viewAllDonors()
{
    const std::string sql =
        "SELECT donorID, username, name, age, gender, bloodType, contact, createdAt "
        "FROM Donor ORDER BY createdAt DESC;";

    viewByQuery<DonorInfo>(
        sql,
        "🧑‍🤝‍🧑 Registered Donors",
        extractDonorRow);
}

void Donor::findDonorsByBloodType(const std::string &bloodType)
{
    const std::string sql =
        "SELECT donorID, username, name, age, gender, bloodType, contact, createdAt "
        "FROM Donor WHERE bloodType = ? ORDER BY name ASC;";

    viewByQuery<DonorInfo>(
        sql,
        "🔎 Donors with blood type " + bloodType,
        extractDonorRow,
        bloodType);
}

int Donor::getDonorIDByUsername(const std::string &username)
{
    sqlite3 *db = Database::getDB();
    std::string sql = "SELECT donorID FROM Donor WHERE username = ? LIMIT 1;";
    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int donorID = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        donorID = sqlite3_column_int(stmt, 0);
    }
    else
    {
        std::cerr << "❌ Donor \"" << username << "\" not found.\n";
    }

    sqlite3_finalize(stmt);
    return donorID;
}
