#include "../utils/httplib.h"
#include "../db/Database.h"
#include "../utils/json.hpp"
#include "../core/Recipient.h"
#include "../core/Donor.h"
#include "../core/DonationRequest.h"

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
   
// Allow preflight OPTIONS requests
svr.Options(R"(.*)", [](const Request& req, Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.status = 204; // No Content
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

    svr.Post("/donation-request", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string name = data["fullName"];  // ✅ FIXED here
            std::string bloodType = data["bloodType"];
            int age = data["age"];
            std::string gender = data["gender"];
            std::string contact = data["contact"];
            int hospitalID = data["hospitalID"];
            std::string scheduledDate = data["scheduledDate"];
    
            bool success = DonationRequest::createRequest(name, bloodType, age, gender, contact, hospitalID, scheduledDate);
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(R"({"success":false,"message":"Invalid input"})", "application/json");
        }
    });
    
    
    svr.Get("/donation-requests/pending", [](const Request& req, Response& res) {
        json result = json::array();
        sqlite3* db = Database::getDB();
    
        std::string sql = R"(
            SELECT D.requestID, D.fullName, D.bloodType, D.age, D.gender, D.contact,
                   H.name AS hospitalName, D.scheduledDate, D.fulfilled
            FROM DonationRequest D
            JOIN Hospital H ON D.hospitalID = H.hospitalID
            WHERE D.fulfilled = 0
            ORDER BY D.scheduledDate ASC
        )";
    
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"fullName", (const char*)sqlite3_column_text(stmt, 1)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 2)},
                {"age", sqlite3_column_int(stmt, 3)},
                {"gender", (const char*)sqlite3_column_text(stmt, 4)},
                {"contact", (const char*)sqlite3_column_text(stmt, 5)},
                {"hospitalName", (const char*)sqlite3_column_text(stmt, 6)},
                {"scheduledDate", (const char*)sqlite3_column_text(stmt, 7)},
                {"fulfilled", sqlite3_column_int(stmt, 8)}
            });
        }
        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(result.dump(), "application/json");
    });

    svr.Get("/donation-requests/fulfilled", [](const Request& req, Response& res) {
        json result = json::array();
        sqlite3* db = Database::getDB();
    
        std::string sql = R"(
            SELECT D.requestID, D.fullName, D.bloodType, D.age, D.gender, D.contact,
                   H.name AS hospitalName, D.scheduledDate, D.fulfilled
            FROM DonationRequest D
            JOIN Hospital H ON D.hospitalID = H.hospitalID
            WHERE D.fulfilled = 1
            ORDER BY D.scheduledDate DESC
        )";
    
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"fullName", (const char*)sqlite3_column_text(stmt, 1)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 2)},
                {"age", sqlite3_column_int(stmt, 3)},
                {"gender", (const char*)sqlite3_column_text(stmt, 4)},
                {"contact", (const char*)sqlite3_column_text(stmt, 5)},
                {"hospitalName", (const char*)sqlite3_column_text(stmt, 6)},
                {"scheduledDate", (const char*)sqlite3_column_text(stmt, 7)},
                {"fulfilled", sqlite3_column_int(stmt, 8)}
            });
        }
        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(result.dump(), "application/json");
    });

    svr.Post(R"(/donation-request/(\d+)/fulfill)", [](const Request& req, Response& res) {
        int requestID = std::stoi(req.matches[1]);
    
        try {
            auto data = json::parse(req.body);
            std::string bloodType = data["bloodType"];
            int quantity = data["quantity"];
    
            bool success = DonationRequest::fulfillRequest(requestID, bloodType, quantity);
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(R"({"success":false,"message":"Invalid input"})", "application/json");
        }
    });
    
    svr.Post("/recipient", [](const Request& req, Response& res) {
        try {
            auto data = json::parse(req.body);
            std::string name = data["name"];
            std::string bloodType = data["bloodType"];
            std::string urgency = data["urgency"];
            std::string contact = data["contact"];
            int hospitalID = data["hospitalID"];
    
            bool success = Recipient::addRecipient(name, bloodType, urgency, contact, hospitalID);
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(json({{"success", success}}).dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Origin", "*");
res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.set_content(R"({"success":false,"message":"Invalid input"})", "application/json");
        }
    });
    svr.Get("/recipients", [](const Request& req, Response& res) {
        json result = json::array();
        sqlite3* db = Database::getDB();
        std::string sql =
            "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
            "FROM Recipient ORDER BY createdAt DESC;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"name", (const char*)sqlite3_column_text(stmt, 1)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 2)},
                {"urgency", (const char*)sqlite3_column_text(stmt, 3)},
                {"contact", (const char*)sqlite3_column_text(stmt, 4)},
                {"hospitalID", sqlite3_column_int(stmt, 5)},
                {"fulfilled", sqlite3_column_int(stmt, 6)}
            });
        }
        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(result.dump(), "application/json");
    });
    svr.Get("/recipients/unfulfilled", [](const Request& req, Response& res) {
        json result = json::array();
        sqlite3* db = Database::getDB();
        std::string sql =
            "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
            "FROM Recipient WHERE fulfilled=0 ORDER BY urgencyLevel DESC;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"name", (const char*)sqlite3_column_text(stmt, 1)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 2)},
                {"urgency", (const char*)sqlite3_column_text(stmt, 3)},
                {"contact", (const char*)sqlite3_column_text(stmt, 4)},
                {"hospitalID", sqlite3_column_int(stmt, 5)},
                {"fulfilled", sqlite3_column_int(stmt, 6)}
            });
        }
        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(result.dump(), "application/json");
    });
            
    svr.Get("/recipients/urgent", [](const Request& req, Response& res) {
        json result = json::array();
        sqlite3* db = Database::getDB();
        std::string sql =
            "SELECT recipientID,name,bloodType,urgencyLevel,contact,hospitalID,fulfilled "
            "FROM Recipient WHERE urgencyLevel = 'High' AND fulfilled = 0 ORDER BY createdAt DESC;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"name", (const char*)sqlite3_column_text(stmt, 1)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 2)},
                {"urgency", (const char*)sqlite3_column_text(stmt, 3)},
                {"contact", (const char*)sqlite3_column_text(stmt, 4)},
                {"hospitalID", sqlite3_column_int(stmt, 5)},
                {"fulfilled", sqlite3_column_int(stmt, 6)}
            });
        }
        sqlite3_finalize(stmt);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(result.dump(), "application/json");
    });
    

    svr.Put(R"(/recipient/(\d+))", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        auto data = json::parse(req.body);
    
        bool success = Recipient::updateRecipient(
            id,
            data["name"],
            data["bloodType"],
            data["urgency"],
            data["contact"],
            data["hospitalID"]
        );
    res.set_header("Access-Control-Allow-Origin", "*");
res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(json({{"success", success}}).dump(), "application/json");
    });
    svr.Delete(R"(/recipient/(\d+))", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        bool success = Recipient::deleteRecipient(id);
    
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(json({{"success", success}}).dump(), "application/json");
    });
    svr.Post(R"(/recipient/(\d+)/fulfill)", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        auto data = json::parse(req.body);
        int quantity = data["quantity"];
    
        Recipient::fulfillRecipientNeed(id, quantity);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(json({{"success", true}}).dump(), "application/json");
    });
            
// ✅ Register a new donor using full logic
svr.Post("/donor", [](const Request& req, Response& res) {
    try {
        auto data = json::parse(req.body);

        std::string username = data["username"];
        std::string name = data["name"];
        int age = data["age"];
        std::string gender = data["gender"];
        std::string bloodType = data["bloodType"];
        std::string contact = data["contact"];

        bool success = Donor::addDonor(username, name, age, gender, bloodType, contact);

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(json({{"success", success}}).dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(R"({"success": false, "message": "Invalid request body"})", "application/json");
    }
});
// DELETE donor
svr.Delete(R"(/donor/(\d+))", [](const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    bool success = Donor::deleteDonor(id);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(json({{"success", success}}).dump(), "application/json");
});
svr.Get("/donors", [](const Request& req, Response& res) {
    sqlite3* db = Database::getDB();
    std::string query = "SELECT donorID, username, name, age, gender, bloodType, contact, createdAt FROM Donor";
    sqlite3_stmt* stmt;

    json donors = json::array();

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            donors.push_back({
                {"id", sqlite3_column_int(stmt, 0)},
                {"username", (const char*)sqlite3_column_text(stmt, 1)},
                {"name", (const char*)sqlite3_column_text(stmt, 2)},
                {"age", sqlite3_column_int(stmt, 3)},
                {"gender", (const char*)sqlite3_column_text(stmt, 4)},
                {"bloodType", (const char*)sqlite3_column_text(stmt, 5)},
                {"contact", (const char*)sqlite3_column_text(stmt, 6)},
                {"createdAt", (const char*)sqlite3_column_text(stmt, 7)}
            });
        }
        sqlite3_finalize(stmt);

        res.set_content(donors.dump(), "application/json");
        res.status = 200;
    } else {
        res.status = 500;
        res.set_content(R"({"success": false, "message": "Query failed"})", "application/json");
    }

    // ✅ Always include CORS headers
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
});



// Filter by blood type
svr.Get(R"(/donors/type/([\w+-]+))", [](const Request& req, Response& res) {
    std::string bloodType = req.matches[1];
    json result = json::array();
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT donorID, username, name, age, gender, bloodType, contact, createdAt FROM Donor WHERE bloodType = ? ORDER BY name;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, bloodType.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back({
            {"id", sqlite3_column_int(stmt, 0)},
            {"username", (const char*)sqlite3_column_text(stmt, 1)},
            {"name", (const char*)sqlite3_column_text(stmt, 2)},
            {"age", sqlite3_column_int(stmt, 3)},
            {"gender", (const char*)sqlite3_column_text(stmt, 4)},
            {"bloodType", (const char*)sqlite3_column_text(stmt, 5)},
            {"contact", (const char*)sqlite3_column_text(stmt, 6)},
            {"createdAt", (const char*)sqlite3_column_text(stmt, 7)}
        });
    }
    sqlite3_finalize(stmt);
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(result.dump(), "application/json");
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
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

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

// GET all hospitals
svr.Get("/hospitals", [](const Request& req, Response& res) {
    sqlite3* db = Database::getDB();
    std::string sql = "SELECT hospitalID, name, location, contact, createdAt FROM Hospital";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        res.status = 500;
        res.set_content(R"({"success": false})", "application/json");
        return;
    }

    json result = json::array();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back({
            {"id", sqlite3_column_int(stmt, 0)},
            {"name", (const char*)sqlite3_column_text(stmt, 1)},
            {"location", (const char*)sqlite3_column_text(stmt, 2)},
            {"contact", (const char*)sqlite3_column_text(stmt, 3)},
            {"createdAt", (const char*)sqlite3_column_text(stmt, 4)}
        });
        
    }

    sqlite3_finalize(stmt);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(result.dump(), "application/json");
});

// DELETE a hospital
svr.Delete(R"(/hospital/(\d+))", [](const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    sqlite3* db = Database::getDB();
    std::string sql = "DELETE FROM Hospital WHERE hospitalID = ?";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(json({{"success", success}}).dump(), "application/json");
});

    // ✅ Start the server
// POST /hospital — Add Hospital
svr.Post("/hospital", [](const Request& req, Response& res) {
    try {
        auto data = json::parse(req.body);
        std::string name = data["name"];
        std::string location = data["location"];
        std::string contact = data["contact"];  // ⬅️ NEW FIELD

        sqlite3* db = Database::getDB();
        std::string sql = "INSERT INTO Hospital (name, location, contact) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            res.status = 500;
            res.set_content(R"({"success": false, "message": "Prepare failed"})", "application/json");
            return;
        }

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, contact.c_str(), -1, SQLITE_TRANSIENT);  // ⬅️ BIND CONTACT

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(json({{"success", success}}).dump(), "application/json");

    } catch (...) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = 400;
        res.set_content(R"({"success": false, "message": "Invalid input"})", "application/json");
    }
});
// PUT /hospital/:id — Update Hospital
svr.Put(R"(/hospital/(\d+))", [](const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    auto data = json::parse(req.body);

    std::string name = data["name"];
    std::string location = data["location"];
    std::string contact = data["contact"];  // ⬅️ NEW

    sqlite3* db = Database::getDB();
    std::string sql = "UPDATE Hospital SET name = ?, location = ?, contact = ? WHERE hospitalID = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        res.status = 500;
        res.set_content(R"({"success": false, "message": "Prepare failed"})", "application/json");
        return;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, contact.c_str(), -1, SQLITE_TRANSIENT);  // ⬅️ BIND CONTACT
    sqlite3_bind_int(stmt, 4, id);

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
