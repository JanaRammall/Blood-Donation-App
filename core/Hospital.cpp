#include "Hospital.h"
#include "../db/Database.h"
#include "HospitalInfo.h"
#include "../utils/TemplateUtils.h"
#include <iostream>

bool Hospital::addHospital(const std::string& name, const std::string& location, const std::string& contact) {
    sqlite3* db = Database::getDB();

    if (Database::recordExists("Hospital", "name", name)) {
        std::cerr << "⚠️ Hospital already exists: " << name << "\n";
        return false;
    }

    std::string sql = "INSERT INTO Hospital (name, location, contact) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contact.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "✅ Hospital \"" << name << "\" added successfully.\n";
        sqlite3_finalize(stmt);
        return true;
    } else {
        std::cerr << "❌ Failed to add hospital: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

void Hospital::viewAllHospitals() {
    sqlite3* db = Database::getDB();
    sqlite3_stmt* stmt;
    std::vector<HospitalInfo> hospitals;

    std::string sql = "SELECT * FROM Hospital ORDER BY name ASC";
    Database::prepareStatement(db, stmt, sql);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hospitals.emplace_back(
            sqlite3_column_int(stmt, 0),
            (const char*)sqlite3_column_text(stmt, 1),
            (const char*)sqlite3_column_text(stmt, 2),
            (const char*)sqlite3_column_text(stmt, 3)
        );
    }

    sqlite3_finalize(stmt);
    printList(hospitals, "🏥 Registered Hospitals");
}

int Hospital::getHospitalIDByName(const std::string& name) {
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT hospitalID FROM Hospital WHERE name = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    int hospitalID = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        hospitalID = sqlite3_column_int(stmt, 0);
    } else {
        std::cerr << "❌ Hospital \"" << name << "\" not found.\n";
    }

    sqlite3_finalize(stmt);
    return hospitalID;
}

std::string Hospital::getHospitalNameByID(int hospitalID) {
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT name FROM Hospital WHERE hospitalID = ?";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_int(stmt, 1, hospitalID);

    std::string name = "Unknown";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        name = (const char*)sqlite3_column_text(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return name;
}
