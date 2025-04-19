#include "../utils/httplib.h"
#include "../db/Database.h"
#include "../utils/json.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <sqlite3.h>

using json = nlohmann::json;
using namespace httplib;

// 🔐 Hashing function
std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

int main() {
    Server svr;

    // ✅ Handle CORS preflight requests
    svr.Options(R"(.*)", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204; // No Content
    });
    svr.Post("/donor", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string query = "INSERT INTO Donor (username, name, bloodtype, contact) VALUES (?, ?, ?, ?)";
            sqlite3_stmt* stmt;
            sqlite3* db = Database::getDB();
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, data["username"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, data["name"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, data["bloodtype"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, data["contact"].get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
            bool success = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
    
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content(R"({"success":false})", "application/json");
        }
    });
    
    // ✅ Handle /login request
    svr.Post("/login", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string username = data["username"];
            std::string password = data["password"];
            std::string hashedInput = sha256(password);

            std::string query = "SELECT password, role FROM User WHERE username = ?";
            sqlite3_stmt* stmt = nullptr;
            sqlite3* db = Database::getDB();

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                res.set_header("Access-Control-Allow-Origin", "*");
                res.status = 500;
                res.set_content(R"({"success": false, "message": "Database prepare failed"})", "application/json");
                return;
            }

            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

            bool success = false;
            std::string role;

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string dbPass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                if (dbPass == hashedInput) {
                    role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                    success = true;
                }
            }

            sqlite3_finalize(stmt);

            res.set_header("Access-Control-Allow-Origin", "*");

            if (success) {
                json resJson = { {"success", true}, {"role", role} };
                res.set_content(resJson.dump(), "application/json");
            } else {
                res.status = 401;
                res.set_content(R"({"success": false, "message": "Invalid credentials"})", "application/json");
            }

        } catch (...) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 400;
            res.set_content(R"({"success": false, "message": "Invalid request"})", "application/json");
        }
    });

    std::cout << "🟢 Server is running on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
