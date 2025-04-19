// js/main.js
document.getElementById("loginForm").addEventListener("submit", async (e) => {
    e.preventDefault();
    const username = document.getElementById("username").value.trim();
    const password = document.getElementById("password").value.trim();
  
    const response = await fetch("http://localhost:8080/login", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username, password }),
    });
  
    const data = await response.json();
  
    if (data.success) {
      localStorage.setItem("role", data.role); // Store role for access control
      switch (data.role) {
        case "admin":
          window.location.href = "admin.html";
          break;
        case "staff":
          window.location.href = "staff.html";
          break;
        case "viewer":
          window.location.href = "viewer.html";
          break;
      }
    } else {
      document.getElementById("loginMessage").textContent = data.message || "Login failed.";
    }
  });
  