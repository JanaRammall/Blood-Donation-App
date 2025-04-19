#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
private:
    static sqlite3* db;

public:
    static sqlite3* openDatabase(const std::string& filename = "blood_donation.db");
    static void closeDatabase(sqlite3* db);
    static bool executeSQL(sqlite3* db, const std::string& sql);
    static void createTables(sqlite3* db);

    static void prepareStatement(sqlite3* db, sqlite3_stmt*& stmt, const std::string& sql);
    static bool recordExists(const std::string& table, const std::string& column, const std::string& value);
    static sqlite3* getDB();
};

#endif // DATABASE_H