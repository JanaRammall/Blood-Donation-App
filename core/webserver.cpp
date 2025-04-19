#include "../utils/httplib.h"
#include "../db/Database.h"
#include <iostream>
#include <nlohmann/json.hpp>  // We'll use this for JSON parsing

using json = nlohmann::json;
using namespace httplib;

int main() {
    Server svr;

    svr.Post("/login", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string username = data["username"];
            std::string password = data["password"];

            // Check credentials using your existing SQLite logic
            std::string query = "SELECT password, role FROM User WHERE username = ?";
            sqlite3_stmt* stmt = Database::prepare(query);
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

            bool success = false;
            std::string role;
            std::string hashedInput = Logger::sha256(password); // hash input password

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string dbPass = (const char*)sqlite3_column_text(stmt, 0);
                if (dbPass == hashedInput) {
                    role = (const char*)sqlite3_column_text(stmt, 1);
                    success = true;
                }
            }

            sqlite3_finalize(stmt);

            if (success) {
                json resJson = { {"success", true}, {"role", role} };
                res.set_content(resJson.dump(), "application/json");
            } else {
                res.status = 401;
                res.set_content(R"({"success": false, "message": "Invalid credentials"})", "application/json");
            }

        } catch (...) {
            res.status = 400;
            res.set_content(R"({"success": false, "message": "Invalid request"})", "application/json");
        }
    });

    std::cout << "🟢 Server is running on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
