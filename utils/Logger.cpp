#include "Logger.h"
#include "../db/Database.h"
#include <iostream>

void Logger::log(const std::string& username, const std::string& activity) {
    sqlite3* db = Database::getDB();

    std::string sql = "INSERT INTO Log (timestamp, username, activity) VALUES (datetime('now'), ?, ?);";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, activity.c_str(), -1, SQLITE_TRANSIENT);
    
    std::cout << "[LOGGING] " << username << ": " << activity << std::endl;

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "❌ Logger error: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
}

void Logger::viewLog() {
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT timestamp, username, activity FROM Log ORDER BY timestamp DESC;";
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt, sql);

    std::cout << "📜 Activity Log:\n";
    std::cout << "-------------------------------------------\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* timestamp = sqlite3_column_text(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        const unsigned char* activity = sqlite3_column_text(stmt, 2);

        std::cout << "[" << timestamp << "] "
                  << (username ? (const char*)username : "system")
                  << ": " << activity << "\n";
    }

    sqlite3_finalize(stmt);
}
