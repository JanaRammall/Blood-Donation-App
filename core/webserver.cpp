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

// 🔐 SHA256 Hash Function
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

// ✅ FIXED: Allow DELETE in CORS preflight
svr.Options(R"(.*)", [](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, DELETE");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.status = 204;
});
    svr.Post("/assign_role", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string username = data["username"];
            std::string role = data["role"];
    
            if (role != "admin" && role != "staff" && role != "viewer" && role != "audit") {
                res.status = 400;
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_content(R"({"success": false, "message": "Invalid role"})", "application/json");
                return;
            }
    
            sqlite3* db = Database::getDB();
            std::string query = "UPDATE User SET role = ? WHERE username = ?";
            sqlite3_stmt* stmt;
    
            if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                res.status = 500;
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_content(R"({"success": false, "message": "Prepare failed"})", "application/json");
                return;
            }
    
            sqlite3_bind_text(stmt, 1, role.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
            bool success = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
    
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
    
        } catch (...) {
            res.status = 500;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(R"({"success": false, "message": "Exception occurred"})", "application/json");
        }
    });
    
    // 🔐 Login Handler
    svr.Post("/login", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string username = data["username"];
            std::string password = data["password"];
            std::string hashedInput = sha256(password);

            sqlite3* db = Database::getDB();
            std::string query = "SELECT password, role FROM User WHERE username = ?";
            sqlite3_stmt* stmt = nullptr;

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                res.set_header("Access-Control-Allow-Origin", "*");
                res.status = 500;
                res.set_content(R"({"success": false, "message": "DB prepare failed"})", "application/json");
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
                res.set_content(json({{"success", true}, {"role", role}}).dump(), "application/json");
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

    // 👤 Create Donor
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
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
            res.set_content(R"({"success": false, "message": "Server error"})", "application/json");
        }
    });

    // 🩸 GET /blood
    svr.Get("/blood", [](const Request& req, Response& res) {
        try {
            sqlite3* db = Database::getDB();
            std::string query = "SELECT bloodType, quantity, createdAt FROM BloodUnit";
            sqlite3_stmt* stmt = nullptr;

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                res.status = 500;
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_content(R"({"success": false, "message": "Query failed"})", "application/json");
                return;
            }

            json result = json::array();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string bloodType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                int quantity = sqlite3_column_int(stmt, 1);
                std::string createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

                result.push_back({
                    {"bloodType", bloodType},
                    {"units", quantity},
                    {"updated", createdAt}
                });
            }

            sqlite3_finalize(stmt);
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(result.dump(), "application/json");

        } catch (...) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
            res.set_content(R"({"success": false, "message": "Server error"})", "application/json");
        }
    });
    svr.Post("/create_admin", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string username = data["username"];
            std::string password = data["password"];
            std::string hashed = sha256(password);
    
            sqlite3* db = Database::getDB();
            std::string query = "INSERT INTO User (username, password, role) VALUES (?, ?, 'admin')";
            sqlite3_stmt* stmt;
    
            if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                res.status = 500;
                res.set_content(R"({"success": false, "message": "DB prepare error"})", "application/json");
                return;
            }
    
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_TRANSIENT);
    
            bool success = (sqlite3_step(stmt) == SQLITE_DONE);
            sqlite3_finalize(stmt);
    
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
        } catch (...) {
            res.status = 500;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(R"({"success": false, "message": "Exception"})", "application/json");
        }
    });
    

    // 👥 GET /users
    svr.Get("/users", [](const Request& req, Response& res) {
        sqlite3* db = Database::getDB();
        std::string query = "SELECT username, role FROM User";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
            res.set_content(R"({"error":"Failed to prepare query"})", "application/json");
            return;
        }

        json users = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string role     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            users.push_back({
                {"username", username},
                {"role", role}
            });
        }

        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(users.dump(), "application/json");
    });
// 🗑️ DELETE /user/:username
svr.Delete(R"(/user/(\w+))", [](const Request& req, Response& res) {
    std::string username = req.matches[1];  // Extract username from route
    sqlite3* db = Database::getDB();

    std::string query = "DELETE FROM User WHERE username = ?";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(R"({"success": false, "message": "Prepare failed"})", "application/json");
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(json({{"success", success}}).dump(), "application/json");
});

    // ✅ Start the server
    std::cout << "🟢 Server is running on http://localhost:8080\n";
    if (!svr.listen("0.0.0.0", 8080)) {
        std::cerr << "❌ Failed to bind to port 8080. Try a different port.\n";
        return 1;
    }

    return 0;
}
