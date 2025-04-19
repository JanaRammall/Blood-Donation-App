#ifndef TEMPLATEUTILS_H
#define TEMPLATEUTILS_H

#include <vector>
#include <iostream>

template <typename T>
void printList(const std::vector<T>& items, const std::string& header = "List") {
    std::cout << "\n=== " << header << " (" << items.size() << " items) ===\n";
    for (const auto& item : items) {
        std::cout << "- " << item << "\n";
    }
}

// ─── convenience wrappers ─────────────────────────────────────────────────────
// Safely get a TEXT column as std::string
inline std::string getColText(sqlite3_stmt* stmt, int col) {
    const unsigned char* txt = sqlite3_column_text(stmt, col);
    return txt ? reinterpret_cast<const char*>(txt) : "";
}

// Safely get an INTEGER column
inline int getColInt(sqlite3_stmt* stmt, int col) {
    return sqlite3_column_int(stmt, col);
}

// ─── generic “view by query” helper ──────────────────────────────────────────
template<typename InfoT, typename Extractor>
inline void viewByQuery(const std::string& sql,
                        const std::string& title,
                        Extractor extractor,
                        const std::string& bindValue = "")
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    db = Database::getDB();
    Database::prepareStatement(db, stmt, sql);
    if (!bindValue.empty())
        sqlite3_bind_text(stmt, 1, bindValue.c_str(), -1, SQLITE_STATIC);

    std::vector<InfoT> items;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        items.push_back(extractor(stmt));
    }
    sqlite3_finalize(stmt);
    printList(items, title);
}

#endif
