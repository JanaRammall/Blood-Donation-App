#include "Database.h"
#include <iostream>
#include <sstream>

sqlite3* Database::db = nullptr;

sqlite3* Database::getDB() {
    if (!db) db = openDatabase(); // Ensure one open connection
    return db;
}

sqlite3* Database::openDatabase(const std::string& filename) {
    sqlite3* db;
    if (sqlite3_open(filename.c_str(), &db)) {
        std::cerr << "❌ Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

void Database::closeDatabase(sqlite3* db) {
    if (db) sqlite3_close(db);
}

void Database::prepareStatement(sqlite3* db, sqlite3_stmt*& stmt, const std::string& sql) {
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite Prepare Error: " << sqlite3_errmsg(db) << "\n";
    }
}

bool Database::recordExists(const std::string& table, const std::string& column, const std::string& value) {
    std::string sql = "SELECT 1 FROM " + table + " WHERE " + column + " = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    prepareStatement(getDB(), stmt, sql);
    sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_STATIC);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

bool Database::executeSQL(sqlite3* db, const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQLite Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void Database::createTables(sqlite3* db) {
    std::string sql =

        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS User ("
        "    userID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username TEXT UNIQUE NOT NULL,"
        "    password TEXT NOT NULL,"
        "   role TEXT NOT NULL CHECK(role IN ('admin','staff','viewer','audit')),"
        "    createdAt TEXT DEFAULT (datetime('now'))"
        "  );"
        
        "CREATE TABLE IF NOT EXISTS Donor ("
        "  donorID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE,"
        "  name TEXT,"
        "  age INTEGER,"
        "  gender TEXT,"
        "  bloodType TEXT,"
        "  contact TEXT CHECK(length(contact) >= 7 AND length(contact) <= 15),"
        "  createdAt TEXT DEFAULT (datetime('now'))"
        ");"

        "CREATE TABLE IF NOT EXISTS Hospital ("
        "  hospitalID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT UNIQUE,"
        "  location TEXT,"
        "  contact TEXT CHECK(length(contact) >= 7 AND length(contact) <= 15),"
        "  createdAt TEXT DEFAULT (datetime('now'))"
        ");"

        "CREATE TABLE IF NOT EXISTS Recipient ("
        "  recipientID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT,"
        "  bloodType TEXT,"
        "  urgencyLevel TEXT,"
        "  contact TEXT CHECK(length(contact) >= 7 AND length(contact) <= 15),"
        "  fulfilled BOOLEAN DEFAULT 0,"
        "  hospitalID INTEGER,"
        "  createdAt TEXT DEFAULT (datetime('now')),"
        "  FOREIGN KEY (hospitalID) REFERENCES Hospital(hospitalID)"
        ");"

        "CREATE TABLE IF NOT EXISTS BloodUnit ("
        "  bloodID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  bloodType TEXT,"
        "  quantity INTEGER,"
        "  expirationDate TEXT CHECK(length(expirationDate) = 10),"
        "  sourceRequestID INTEGER,"
        "  createdAt TEXT DEFAULT (datetime('now')),"
        "  FOREIGN KEY (sourceRequestID) REFERENCES DonationRequest(requestID)"
        ");"

        "CREATE TABLE IF NOT EXISTS DonationRequest ("
        "  requestID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  fullName TEXT,"
        "  bloodType TEXT,"
        "  age INTEGER,"
        "  gender TEXT,"
        "  contact TEXT CHECK(length(contact) >= 7 AND length(contact) <= 15),"
        "  hospitalID INTEGER,"
        "  scheduledDate TEXT CHECK(length(scheduledDate) = 10),"
        "  status TEXT DEFAULT 'pending' CHECK(status IN ('pending', 'approved', 'rejected', 'completed')),"
        "  fulfilled BOOLEAN DEFAULT 0,"
        "  createdAt TEXT DEFAULT (datetime('now')),"
        "  FOREIGN KEY (hospitalID) REFERENCES Hospital(hospitalID)"
        ");"

        "CREATE TABLE IF NOT EXISTS Log ("
        "  timestamp TEXT DEFAULT (datetime('now')),"
        "  username TEXT,"
        "  activity TEXT"
        ");"
    ;

    if (!executeSQL(db, sql)) {
        std::cerr << "❌ Table creation failed.\n";
    } else {
        std::cout << "✅ Tables created/updated successfully.\n";
    }
}
