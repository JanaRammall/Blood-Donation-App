#include <iostream>
#include <string>
#ifdef _WIN32
  #include <windows.h>   // for UTF‑8 console
  #include <conio.h>     // for _getch()
#endif

#include "db/Database.h"
#include "core/User.h"
#include "core/UserAccount.h"
#include "core/Admin.h"
#include "core/Staff.h"
#include "core/Viewer.h"
#include "core/Auditor.h"
#include "utils/InputUtils.h"
#include "utils/Logger.h"

int main() {
  #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // Enable UTF‑8 for emoji support
  #endif

  // ─── Setup DB & ensure at least one admin ───────────────────────────────────
  Database::createTables(Database::getDB());
  {
    sqlite3* db = Database::getDB();
    sqlite3_stmt* stmt;
    Database::prepareStatement(db, stmt,
      "SELECT COUNT(*) FROM User WHERE role = 'admin';");
    int hasAdmin = (sqlite3_step(stmt) == SQLITE_ROW)
                   ? sqlite3_column_int(stmt, 0)
                   : 0;
    sqlite3_finalize(stmt);

    if (hasAdmin == 0) {
      std::cout << "🚧 No admin account found. Let's create one.\n";
      std::string uname = inputLine("Admin username: ");
      std::string pwd   = getMaskedPassword("Admin password: ");
      User::registerUser(uname, pwd, "admin");
    }
  }
  // ────────────────────────────────────────────────────────────────────────────


  std::string currentUser;
  std::string currentRole;

  std::cout << "=== 💉 Welcome to the Blood Bank System ===\n";

  // 🔄 Login loop
  while (true) {
    std::cout << "\n1. 🔐 Login\n0. ❌ Exit\nChoice: ";
    int choice = inputInt("");

    if (choice == 0) {
      std::cout << "👋 Goodbye!\n";
      return 0;
    }
    if (choice != 1) {
      std::cout << "❓ Invalid choice. Try again.\n";
      continue;
    }

    // choice == 1: attempt login
    currentUser = inputLine("Username: ");
    std::string password = getMaskedPassword("Password: ");

    if (!User::authenticate(currentUser, password, currentRole)) {
      std::cerr << "❌ Login failed. Please try again.\n";
      continue;
    }

    std::cout << "✅ Welcome, " << currentUser
              << " (" << currentRole << ")\n";
    break;
  }

  // 🔓 Dispatch to the correct role
  UserAccount* current = nullptr;
  if      (currentRole == "admin")  current = new Admin(currentUser);
  else if (currentRole == "staff")  current = new Staff(currentUser);
  else if (currentRole == "viewer") current = new Viewer(currentUser);
  else if (currentRole == "audit")  current = new Auditor(currentUser);
  else {
    std::cerr << "❌ Unknown role: " << currentRole << "\n";
    return 1;
  }

  // 📋 Run the role’s interactive menu
  current->displayMenu();
  delete current;

  return 0;
}