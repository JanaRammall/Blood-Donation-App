#include "BloodUnit.h"
#include "../db/Database.h"
#include "BloodUnitInfo.h"
#include "../utils/TemplateUtils.h"
#include <iostream>

static BloodUnitInfo extractBloodUnitRow(sqlite3_stmt *s)
{
    return BloodUnitInfo{
        getColInt(s, 0),  // bloodID
        getColText(s, 1), // bloodType
        getColInt(s, 2),  // quantity
        getColText(s, 3)  // expirationDate
    };
}

bool BloodUnit::addBloodUnit(const std::string &bloodType,
                             int quantity,
                             const std::string &expirationDate,
                             int sourceRequestID)
{
    sqlite3 *db = Database::getDB();
    std::string sql = "INSERT INTO BloodUnit (bloodType, quantity, expirationDate, sourceRequestID) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    Database::prepareStatement(db, stmt, sql);

    sqlite3_bind_text(stmt, 1, bloodType.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, quantity);
    sqlite3_bind_text(stmt, 3, expirationDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, sourceRequestID);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        std::cout << "✅ Blood unit recorded from request #" << sourceRequestID << "\n";
        sqlite3_finalize(stmt);
        return true;
    }
    else
    {
        std::cerr << "❌ SQLite Insert Error: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

void BloodUnit::getUnitsByType(const std::string &bloodType)
{
    static const std::string sql =
        "SELECT bloodID, bloodType, quantity, expirationDate "
        "FROM BloodUnit "
        "WHERE bloodType = ? "
        "ORDER BY expirationDate ASC;";

    viewByQuery<BloodUnitInfo>(
        sql,
        "🩸 Blood Units for Type: " + bloodType,
        extractBloodUnitRow,
        bloodType);
}

void BloodUnit::removeExpiredUnits()
{
    sqlite3 *db = Database::getDB();
    std::string sql = "DELETE FROM BloodUnit WHERE expirationDate < date('now');";

    if (Database::executeSQL(db, sql))
    {
        std::cout << "🧹 Expired blood units removed.\n";
    }
    else
    {
        std::cerr << "❌ Failed to remove expired blood units.\n";
    }
}

void BloodUnit::listAllAvailable()
{
    static const std::string sql =
        "SELECT bloodID, bloodType, quantity, expirationDate "
        "FROM BloodUnit "
        "ORDER BY expirationDate ASC;";

    viewByQuery<BloodUnitInfo>(
        sql,
        "📦 All Blood Units in Storage",
        extractBloodUnitRow);
}


bool BloodUnit::useBloodUnits(const std::string& bloodType, int quantity) {
    sqlite3* db = Database::getDB();

    const std::string selectSQL =
        "SELECT bloodID, quantity FROM BloodUnit "
        "WHERE bloodType = ? AND expirationDate >= date('now') "
        "ORDER BY expirationDate ASC";

    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, selectSQL);
    sqlite3_bind_text(stmt, 1, bloodType.c_str(), -1, SQLITE_STATIC);

    std::vector<std::pair<int, int>> units;
    int collected = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW && collected < quantity) {
        int id = sqlite3_column_int(stmt, 0);
        int qty = sqlite3_column_int(stmt, 1);
        int take = std::min(qty, quantity - collected);
        collected += take;
        units.emplace_back(id, take);
    }
    sqlite3_finalize(stmt);

    if (collected < quantity)
        return false;

    // Deduct quantities
    for (const auto& [id, used] : units) {
        const std::string updateSQL =
            "UPDATE BloodUnit SET quantity = quantity - ? WHERE bloodID = ?";
        Database::prepareStatement(db, stmt, updateSQL);
        sqlite3_bind_int(stmt, 1, used);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    return true;
}