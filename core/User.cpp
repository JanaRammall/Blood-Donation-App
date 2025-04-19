#include "User.h"
#include "../db/Database.h"
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

bool User::registerUser(const std::string& username,
                        const std::string& password,
                        const std::string& role) {
    sqlite3* db = Database::getDB();

    if (Database::recordExists("User", "username", username)) {
        std::cerr << "⚠️ Username already exists.\n";
        return false;
    }

    std::string sql = "INSERT INTO User (username, password, role) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);

    std::string hashedPass = hashPassword(password);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPass.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "✅ User registered successfully.\n";
        sqlite3_finalize(stmt);
        return true;
    } else {
        std::cerr << "❌ Registration failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }
}

bool User::authenticate(const std::string& username,
                        const std::string& password,
                        std::string& outRole) {
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT password, role FROM User WHERE username = ? LIMIT 1;";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string storedHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);

        std::string inputHash = hashPassword(password);
        if (inputHash == storedHash) {
            outRole = role;
            return true;
        }
    } else {
        sqlite3_finalize(stmt);
    }

    return false;
}